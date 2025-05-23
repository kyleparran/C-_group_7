#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sched.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#include <atomic>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <iostream>
#include <thread>

// UDP/TCP constants
#define MULTICAST_IP "239.255.0.1"
#define SERVER_IP "192.168.50.188"
#define SERVER_PORT 4000
#define CLIENT_PORT 3001
// #define RING_BUFFER_SIZE 65536
#define RING_BUFFER_SIZE 200
#define MAX_LINE_LENGTH 256
#define UTP_CHUNK_SIZE 256


/*
work in progress


# Components
Buffer writer
- only job is to feed UTP data into circular ring buffer

Target finder
- Finds the target ticker string
- Ideally is really close to to target finder.
- Searches backwards
- Responsible for updating ticker finders when a challenge has been solved

Ticker finder
- finds the tickers
- if found then send and update other ticker finders
- one going forward
- one going backward
*/



// Each entry is cache-line aligned to reduce false sharing.
// struct alignas(64) MessageRecord {
//     char data[MAX_LINE_LENGTH];
//     size_t length;
// };
// MessageRecord ringBuffer[RING_BUFFER_SIZE];


// Each entry is cache-line aligned 
alignas(64) std::atomic<char> ringBuffer[RING_BUFFER_SIZE];
std::atomic<size_t> writeIndex(0);

void pinThread(const int core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        perror("sched_setaffinity");
    }
}

// UDP receiver thread:
void writerProcess() {
    pinThread(1);

    // Create and configure the UDP socket.
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    int enableFlag = 1;
    if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &enableFlag, sizeof(enableFlag)) != 0) {
        perror("setsockopt SO_REUSEADDR");
    }
    
    sockaddr_in udpAddr{};
    udpAddr.sin_family = AF_INET;
    udpAddr.sin_port = htons(CLIENT_PORT);
    udpAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(udpSocket, (sockaddr*)&udpAddr, sizeof(udpAddr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    // Join multicast group.
    ip_mreq multicastRequest{};
    multicastRequest.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicastRequest, sizeof(multicastRequest)) < 0) {
        perror("setsockopt IP_ADD_MEMBERSHIP");
    }
    
    // Increase the UDP receive buffer size.
    int receiveBufferSize = 1 << 20;
    if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVBUF, &receiveBufferSize, sizeof(receiveBufferSize)) != 0) {
        perror("setsockopt SO_RCVBUF");
    }
    
    while (true) {
        size_t numToEnd = RING_BUFFER_SIZE - writeIndex.load(std::memory_order_relaxed);
        ssize_t receivedBytes = recv(udpSocket, ringBuffer, numToEnd, 0);
        if (receivedBytes <= 0) continue;
        if (receivedBytes == numToEnd) {
            writeIndex.store(0, std::memory_order_relaxed);
        }
        else {
            writeIndex.store(writeIndex + receivedBytes, std::memory_order_relaxed);
        }
    }
}


/*
// Consumer thread for Security processing.
// It scans new messages directly out of the ring buffer checking for lines starting with "SEC|".
void tickerFinderThread() {
    pinThread(2);
    size_t localReadIndex = 0;
    
    while (running.load(std::memory_order_relaxed)) {
        // While there are new messages in the ring.
        while (localReadIndex < writeIndex.load(std::memory_order_relaxed)) {
            size_t slot = localReadIndex % RING_BUFFER_SIZE;
            MessageRecord &msg = ringBuffer[slot];
            // Check if the message starts with "SEC|".
            if (std::strncmp(msg.data, "SEC|", 4) == 0) {
                // Process security line. For example, print it.
                std::cout << "[SECURITY] " << msg.data << std::endl;
            }
            ++localReadIndex;
        }
        // Busy-wait with a very short delay.
        usleep(50);
    }
}
*/

// thread for finding Challenge/Target
void targetFinderThread() {
    pinThread(3);
    size_t localIndex = 0;

    while (localReadIndex < writeIndex.load(std::memory_order_relaxed)) {
        size_t slot = localReadIndex % RING_BUFFER_SIZE;
        MessageRecord &msg = ringBuffer[slot];
        if (std::strncmp(msg.data, "CHALLENGE_ID:", 13) == 0 || 
            std::strncmp(msg.data, "TARGET:", 7) == 0) {
            // Process challenge/target lines. For example, print it.
            std::cout << "[CHALLENGE/TARGET] " << msg.data << std::endl;
        }
        ++localReadIndex;
    }
}
int main() {
    // Set maximum real-time priority
    struct sched_param schedulingParameters;
    schedulingParameters.sched_priority = 99;
    sched_setscheduler(0, SCHED_FIFO, &schedulingParameters);
    // Lock memory to avoid paging
    mlockall(MCL_CURRENT | MCL_FUTURE);
    // Set highest process priority
    setpriority(PRIO_PROCESS, 0, -20);
    
    // Optionally pin the main thread to CPU core 0.
    pinThread(0);
    
    // Threads.
    std::thread writer(writerProcess);
    // std::thread tickerFinder(tickerFinderThread);
    // std::thread targetFinder(targetFinderThread);
    
    writer.join();
    // tickerFinder.join();
    // targetFinder.join();
    
    return 0;
}
