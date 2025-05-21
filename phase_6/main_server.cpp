#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <random>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <algorithm>

using boost::asio::ip::udp;
using boost::asio::ip::tcp;

struct Challenge {
    int challenge_id;
    std::string target_ticker;
    std::unordered_map<std::string, std::pair<double, double>> market_data;
    bool winner_declared;
    std::string winner_name;
    std::chrono::steady_clock::time_point first_response_time;
};

std::mutex challenge_mutex;
Challenge current_challenge;
std::unordered_map<std::string, int> student_scores;
std::mutex score_mutex;
int global_challenge_id = 0;
constexpr std::size_t MAX_UDP_PAYLOAD = 1400;

// -------------------- Challenge Generation -------------------- //
Challenge generate_challenge(std::mt19937& gen, std::uniform_real_distribution<>& bid_dist, std::uniform_real_distribution<>& inc_dist) {
    Challenge newChallenge;
    newChallenge.challenge_id = ++global_challenge_id;
    newChallenge.winner_declared = false;

    std::vector<std::string> available_tickers;

    // Generate 10 securities
    for (int i = 1; i <= 10; i++) {
        char ticker[10];
        std::snprintf(ticker, sizeof(ticker), "SEC%04d", i);
        double bid = bid_dist(gen);
        double ask = bid + inc_dist(gen);
        newChallenge.market_data[ticker] = {bid, ask};
        available_tickers.push_back(ticker); // Store ticker for selection
    }

    // Select a valid target ticker from the generated tickers
    std::uniform_int_distribution<> ticker_index_dist(0, available_tickers.size() - 1);
    newChallenge.target_ticker = available_tickers[ticker_index_dist(gen)];

    return newChallenge;
}


// -------------------- UDP Market Data Broadcasting -------------------- //
void udp_broadcast_thread(boost::asio::io_context& io_context, unsigned short send_port) {
    try {
        udp::socket socket(io_context, udp::endpoint(udp::v4(), 3000));
        udp::endpoint multicast_endpoint(boost::asio::ip::make_address("239.255.0.1"), 3001);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> bid_dist(10.0, 100.0);
        std::uniform_real_distribution<> inc_dist(0.1, 1.0);
        std::uniform_int_distribution<> target_index_dist(1, 1000);

        while (true) {
            Challenge newChallenge = generate_challenge(gen, bid_dist, inc_dist);


            std::ostringstream msg_stream;
            for (const auto& [ticker, prices] : newChallenge.market_data) {
                msg_stream << "SEC|" << ticker << "|BID|" << prices.first << "|ASK|" << prices.second << "\n";
            }
            msg_stream << "CHALLENGE_ID:" << newChallenge.challenge_id << "\n";
            msg_stream << "TARGET:" << newChallenge.target_ticker << "\n";

            std::string challenge_message = msg_stream.str();
            std::cout << "Challenge Broadcast:\n" << challenge_message << std::endl;

            std::string out_message = msg_stream.str();

            {
                std::lock_guard<std::mutex> lock(challenge_mutex);
                current_challenge = newChallenge;
            }

            std::size_t total_size = out_message.size();
            std::size_t fragment_size = MAX_UDP_PAYLOAD;
            std::size_t num_fragments = (total_size + fragment_size - 1) / fragment_size;

            for (std::size_t i = 0; i < num_fragments; i++) {
                std::size_t start = i * fragment_size;
                std::size_t len = std::min(fragment_size, total_size - start);
                std::string fragment = out_message.substr(start, len);

                boost::system::error_code ec;
                socket.send_to(boost::asio::buffer(fragment), multicast_endpoint, 0, ec);
                if (ec) {
                    std::cerr << "UDP send error: " << ec.message() << std::endl;
                }
            }

            std::cout << "Broadcasted challenge " << newChallenge.challenge_id
                      << " TARGET: " << newChallenge.target_ticker << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    } catch (std::exception& e) {
        std::cerr << "UDP Error: " << e.what() << std::endl;
    }
}

// -------------------- TCP Server for Multi-Client Order Submission -------------------- //
class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    explicit TCPSession(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() { do_read(); }

private:
    void do_read() {
        auto self(shared_from_this());
        boost::asio::async_read_until(socket_, streambuf_, "\n",
            [this, self](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    std::istream is(&streambuf_);
                    std::string line;
                    std::getline(is, line);
                    if (!line.empty()) {
                        process_order(line);
                    }
                    do_read();
                }
            });
    }

    void process_order(const std::string& msg) {
        std::cout << "📩 Received order: " << msg << std::endl;

        std::istringstream iss(msg);
        std::string header, client_name, ticker;
        int challenge_id;
        double submitted_bid, submitted_ask;
        iss >> header >> challenge_id >> ticker >> submitted_bid >> submitted_ask >> client_name;
        // std::cout << "header: " << header << std::endl;
        // std::cout << "Challenge ID: " << challenge_id << std::endl;
        // std::cout << "Ticker: " << ticker << std::endl;
        // std::cout << "Submitted Bid: " << submitted_bid << std::endl;
        // std::cout << "Submitted Ask: " << submitted_ask << std::endl;
        // std::cout << "Client Name: " << client_name << std::endl;

        if (header != "CHALLENGE_RESPONSE") return;

        bool correct = false;
        {
            std::lock_guard<std::mutex> lock(challenge_mutex);

            if (current_challenge.challenge_id == challenge_id && !current_challenge.winner_declared) {
                if (current_challenge.target_ticker == ticker) {
                    auto it = current_challenge.market_data.find(ticker);
                    if (it != current_challenge.market_data.end()) {
                        double expected_bid = it->second.first;
                        double expected_ask = it->second.second;
                        double tol = 1e-3;  // Allows for small rounding errors
                        if (std::abs(submitted_bid - expected_bid) <= tol &&
                                 std::abs(submitted_ask - expected_ask) <= tol)
                        {

                            if (!current_challenge.winner_declared) {  // Ensure only the first correct submission wins
                                current_challenge.winner_declared = true;
                                current_challenge.winner_name = client_name;
                                current_challenge.first_response_time = std::chrono::steady_clock::now();

                                std::cout << "🏆 Challenge " << challenge_id << " won by " << client_name << "!\n";
                            }
                            }


                    }
                }
            }
        }

        std::string response = correct ? ("WINNER " + client_name + "\n") : "INCORRECT\n";
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(response),
            [this, self](boost::system::error_code ec, std::size_t) {
                if (ec) {
                    std::cerr << "TCP send error: " << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    boost::asio::streambuf streambuf_;
};

class TCPServer {
public:
    TCPServer(boost::asio::io_context& io_context, unsigned short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::cout << "✅ Client connected: " << socket.remote_endpoint() << std::endl;
                    std::make_shared<TCPSession>(std::move(socket))->start();
                } else {
                    std::cerr << "❌ Error accepting connection: " << ec.message() << std::endl;
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

// -------------------- Main Function -------------------- //
int main() {
    try {
        boost::asio::io_context io_context;
        std::thread udp_broadcast(udp_broadcast_thread, std::ref(io_context), 3001);
        TCPServer tcp_server(io_context, 4000);
        io_context.run();
        udp_broadcast.join();
    } catch (std::exception& e) {
        std::cerr << "Exception in main: " << e.what() << "\n";
    }
    return 0;
}
