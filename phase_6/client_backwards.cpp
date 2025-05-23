#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sched.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <iostream>
#include <unistd.h>
#include <atomic>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <unordered_map>

#define MULTICAST_IP "239.255.0.1"
#define SERVER_IP "192.168.50.188"
#define SERVER_PORT 4000
#define CLIENT_PORT 3001
#define CLIENT_NAME "Scott-Kyle"
#define BUFFER_SIZE 2048

/* 
this assumes sec prices always come before targetId and challenge id

todo
- handle multiple challenges better
- adjust if challenge is won by someone else
 */

inline void parseAndSend(const char* secPtr, const int tcpSocket, const char* targetSec, const char* challengeId, const int challengeIdLen) {
    // parse bid
    const char* bidStart = secPtr + 17;
    const char* bidEnd = strchr(bidStart, '|');
    int bidLength = bidEnd - bidStart;
    // parse ask
    const char* askStart = bidEnd + 5;
    const char* askEnd = strchr(askStart, '\n');
    int askLength = askEnd - askStart;

    // Prepare the send buffer
    char sendBuffer[128] __attribute__((aligned(64)));
    int bytesToSend = snprintf(
        sendBuffer,
        sizeof(sendBuffer),
        "CHALLENGE_RESPONSE %.*s SEC%.*s %.*s %.*s %s\n",
        challengeIdLen, challengeId,
        4, targetSec,
        bidLength, bidStart,
        askLength, askStart,
        CLIENT_NAME
    );

    send(tcpSocket, sendBuffer, bytesToSend, MSG_NOSIGNAL);
    // std ::cout << "Sent: " << sendBuffer << "\n";
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
    // Pin this process to CPU 0 to reduce scheduling jitter
    cpu_set_t cpuSet;
    CPU_ZERO(&cpuSet);
    CPU_SET(0, &cpuSet);
    sched_setaffinity(0, sizeof(cpuSet), &cpuSet);

    // Create a UDP socket for receiving data
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    int enableFlag = 1;  // Allow reuse of local addresses
    setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &enableFlag, sizeof(enableFlag));
    // Bind UDP socket to client port
    sockaddr_in udpSockaddrIn{};
    udpSockaddrIn.sin_family = AF_INET;
    udpSockaddrIn.sin_port = htons(CLIENT_PORT);
    udpSockaddrIn.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(udpSocket, (sockaddr *)&udpSockaddrIn, sizeof(udpSockaddrIn));
    // Join multicast group
    ip_mreq multicastRequest{};
    multicastRequest.imr_multiaddr.s_addr = inet_addr(MULTICAST_IP);
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicastRequest, sizeof(multicastRequest));
    // Increase receive buffer to handle high throughput
    int receiveBufferSize = 1 << 20;
    setsockopt(udpSocket, SOL_SOCKET, SO_RCVBUF, &receiveBufferSize, sizeof(receiveBufferSize));
    // Create a TCP socket for sending challenge responses
    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(tcpSocket, IPPROTO_TCP, TCP_NODELAY, &enableFlag, sizeof(enableFlag));
    // Setup sockaddr for connecting to TCP server
    sockaddr_in tcpSockaddrIn{};
    tcpSockaddrIn.sin_family = AF_INET;
    tcpSockaddrIn.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &tcpSockaddrIn.sin_addr);
    // Try connecting up to 10 times with short delay in between
    int attempt = 0;
    for (; attempt < 10 && connect(tcpSocket, (sockaddr *)&tcpSockaddrIn, sizeof(tcpSockaddrIn)) < 0; attempt++) {
        std::cout << "Failed to connect to TCP server. Attempt " << attempt + 1 << "/10\n";
        usleep(100000);
    }
    if (attempt == 10) {
        std::cerr << "Failed to connect to TCP server after 10 attempts.\n";
        return -1;
    }
    std::cout << "CONNECTED!\n"; 

    char receiveBuffer[BUFFER_SIZE] __attribute__((aligned(64)));
    const char* targetSec = nullptr;
    const char* challengeId = nullptr;
    int challengeIdLen = -1;
    while (true) {
        // Receive data from UDP
        ssize_t receivedBytes = recv(udpSocket, receiveBuffer, sizeof(receiveBuffer) - 1, 0);
        if (receivedBytes <= 0) continue;

        const char *currPosition = receiveBuffer + receivedBytes;
        const char *startPosition = receiveBuffer;
        while (currPosition >= startPosition) {
            if (*currPosition != '\n'){
                --currPosition;
                continue;
            }
 
            if (!memcmp(currPosition, "\nS", 2)) {
                const char* sec = currPosition + 8;
                if (!memcmp(sec, targetSec, 4))
                    parseAndSend(currPosition, tcpSocket, targetSec, challengeId, challengeIdLen);
                currPosition -= 30;
            }
            else if (!memcmp(currPosition, "\nC", 2)) {
                const char* idStart = currPosition + 14;
                const char* idEnd = strchr(idStart, '\n');
                challengeId = idStart;
                challengeIdLen = idEnd - idStart;
                currPosition -= 30;
            }
            else if(!memcmp(currPosition, "\nT", 2)) {
                targetSec = currPosition + 11;
                currPosition -= 15;
            }
            else {
                --currPosition;
            }
        }
    }
}