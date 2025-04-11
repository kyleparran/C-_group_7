#include <iostream>
#include <thread>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <deque>
#include <unistd.h>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
const int BUFFER_SIZE = (getenv("BUFFER_SIZE") ? stoi(getenv("BUFFER_SIZE")) : 256);
const int MIN_DELAY = (getenv("MIN_DELAY") ? stoi(getenv("MIN_DELAY")) : 100);
const int RANDOM_DELAY = (getenv("RANDOM_DELAY") ? stoi(getenv("RANDOM_DELAY")) : 300);
mutex coutMutex; 

inline void getNextPrice(char* priceBuffer, const int socketFd, int& currPriceIDLen, float& currPrice){
        memset(priceBuffer, 0, BUFFER_SIZE);  // Clear buffer
        int bytesReceived = recv(socketFd, priceBuffer, BUFFER_SIZE - 1, 0); // waits here for a new price
        if (bytesReceived <= 0){
            {
            lock_guard<mutex> lock(coutMutex);
            cerr << "🚨 Server closed connection or error occurred." << endl;
            }
            close(socketFd);
            std::exit(EXIT_FAILURE);
        }

        // Parse price
        char* commaPtr = strchr(priceBuffer, ',');
        if (!commaPtr){
            {
            lock_guard<mutex> lock(coutMutex);
            cerr << "❌ Invalid price format received: " << priceBuffer << endl;
            }
            getNextPrice(priceBuffer, socketFd, currPriceIDLen, currPrice); // retry
            return;
        }

        currPriceIDLen = commaPtr - priceBuffer;
        currPrice = strtof(commaPtr + 1, nullptr);
}

void receiveAndRespond(int socketFd, const string& name){
    char priceBuffer[BUFFER_SIZE];
    int currPriceIDLen = -1;
    float currPrice = -1.0f;
    float lastPrice = -1.0f;
    float goingSign = 1.0f;
    string_view currPriceID(priceBuffer, currPriceIDLen);
    deque<float> priceHistory;

    // Send client name
    send(socketFd, name.c_str(), name.size(), 0);

    // Get first price
    getNextPrice(priceBuffer, socketFd, currPriceIDLen, currPrice);
    currPriceID = string_view(priceBuffer, currPriceIDLen);
    {
    lock_guard<mutex> lock(coutMutex);
    cout << "📲 Received price ID: " << currPriceID << ", Value: " << currPrice << endl;
    }
    priceHistory.push_back(currPrice);

    // Get second price
    getNextPrice(priceBuffer, socketFd, currPriceIDLen, currPrice);
    currPriceID = string_view(priceBuffer, currPriceIDLen);
    {
    lock_guard<mutex> lock(coutMutex);
    cout << "📲 Received price ID: " << currPriceID << ", Value: " << currPrice << endl;
    }
    priceHistory.push_back(currPrice);

    goingSign = (currPrice > priceHistory[0]) ? 1.0f : -1.0f;
    lastPrice = currPrice;
    while (true){
        getNextPrice(priceBuffer, socketFd, currPriceIDLen, currPrice);

        // Trade Detection
        if (goingSign * (currPrice - lastPrice) > 0){
            send(socketFd, priceBuffer, currPriceIDLen, 0);
            this_thread::sleep_for(chrono::milliseconds(MIN_DELAY + rand() % RANDOM_DELAY));

            currPriceID = string_view(priceBuffer, currPriceIDLen);
            {
            lock_guard<mutex> lock(coutMutex);
            cout << "📲 Received price ID: " << currPriceID << ", Value: " << currPrice << endl;
            }
            if(goingSign > 0){
                lock_guard<mutex> lock(coutMutex);
                cout << "📈 Momentum up! Sent order for price ID " << currPriceID << endl;
            }else{
                lock_guard<mutex> lock(coutMutex);
                cout << "📉 Momentum down! Sent order for price ID " << currPriceID << endl;
            }
        }else{lock_guard<mutex> lock(coutMutex);
            cout << "📲 Received price ID: " << currPriceID << ", Value: " << currPrice << endl;
            cout << "➖ No momentum. Ignoring price ID " << currPriceID << endl;
            goingSign = goingSign * -1; // flip the momentum
        }

        // Update price history
        if (priceHistory.size() >= 3)
            priceHistory.pop_front();
        priceHistory.push_back(currPrice);
        lastPrice = currPrice;

    }

    close(socketFd);
}

int main(){
    srand(time(nullptr));

    string name;
    cout << "Enter your client name: ";
    getline(cin, name);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        cerr << "❌ Socket creation failed!" << endl;
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
        cerr << "❌ Connection to server failed!" << endl;
        return 1;
    }

    cout << "✅ Connected to server at " << SERVER_IP << ":" << SERVER_PORT << endl;
    receiveAndRespond(sock, name);
    return 0;
}
