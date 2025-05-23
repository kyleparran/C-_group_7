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

static double bids[1001], asks[1001];
static inline int ti(const char *ptr) { 
    return (ptr[0] - '0') * 1000 + (ptr[1] - '0') * 100 + (ptr[2] - '0') * 10 + (ptr[3] - '0'); 
}

static inline double parsePrice(const char *&ptr)
{
    double wholeNum = 0, decimal = 0;
    int sign = 1, decimalCount = 0;

    // Determine sign
    if (*ptr == '-')
    {
        sign = -1;
        ptr++;
    }

    // Parse number
    for (; *ptr >= '0' && *ptr <= '9'; ptr++)
        wholeNum = wholeNum * 10 + (*ptr - '0');
    if (*ptr == '.')
    {
        ptr++;
        for (; *ptr >= '0' && *ptr <= '9'; ptr++)
        {
            decimal = decimal * 10 + (*ptr - '0');
            decimalCount++;
        }
    }
    while (decimalCount--)
        decimal *= 0.1;
    return sign * (wholeNum + decimal);
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
    // Bind UDP socket to port 3001
    sockaddr_in udpSockaddrIn{};
    udpSockaddrIn.sin_family = AF_INET;
    udpSockaddrIn.sin_port = htons(3001);
    udpSockaddrIn.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(udpSocket, (sockaddr *)&udpSockaddrIn, sizeof(udpSockaddrIn));
    // Join multicast group 239.255.0.1
    ip_mreq multicastRequest{};
    multicastRequest.imr_multiaddr.s_addr = inet_addr("239.255.0.1");
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(udpSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicastRequest, sizeof(multicastRequest));
    // Increase receive buffer to handle high throughput
    int receiveBufferSize = 1 << 20;
    setsockopt(udpSocket, SOL_SOCKET, SO_RCVBUF, &receiveBufferSize, sizeof(receiveBufferSize));
    // Create a TCP socket for sending challenge responses
    int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(tcpSocket, IPPROTO_TCP, TCP_NODELAY, &enableFlag, sizeof(enableFlag));
    // Setup sockaddr for connecting to TCP server on localhost:4000
    sockaddr_in tcpSockaddrIn{};
    tcpSockaddrIn.sin_family = AF_INET;
    tcpSockaddrIn.sin_port = htons(4000);
    inet_pton(AF_INET, "127.0.0.1", &tcpSockaddrIn.sin_addr);
    // Try connecting up to 10 times with short delay in between
    for (int attempt = 0; attempt < 10 && connect(tcpSocket, (sockaddr *)&tcpSockaddrIn, sizeof(tcpSockaddrIn)) < 0; attempt++) {
        usleep(100000);
    }

    
    char receiveBuffer[2048];
    int challengeId = -1, targetSecurityIndex = -1;
    while (true) {
        // Receive data from UDP
        ssize_t receivedBytes = recv(udpSocket, receiveBuffer, sizeof(receiveBuffer) - 1, 0);
        if (receivedBytes <= 0) continue;

        // Null-terminate the received data
        receiveBuffer[receivedBytes] = 0;  // todo delete?

        // Setup pointers to scan through the buffer
        const char *currentPosition = receiveBuffer;
        const char *endPosition = currentPosition + receivedBytes;

        // Parse line by line
        while (currentPosition < endPosition) {
            const char *lineBreakPtr = (const char *)memchr(currentPosition, '\n', endPosition - currentPosition);
            size_t lineSize = lineBreakPtr ? (size_t)(lineBreakPtr - currentPosition) 
                                           : (size_t)(endPosition - currentPosition);

            // Check for SEC line containing bid/ask updates
            if (lineSize >= 4 && !memcmp(currentPosition, "SEC|", 4)) {
                // Example format: SEC|XYZ|...
                const char *tokenPtr = currentPosition + 4;

                // Get security index from 4 chars after "SEC|"
                int securityIndex = ti(tokenPtr + 3);

                // Locate the last '|' to parse the ask price
                const char *lineEndPtr = currentPosition + lineSize;
                while (lineEndPtr > currentPosition && *--lineEndPtr != '|');

                // Move backwards to find the second to last '|'
                const char *secondToLastDelimPtr = lineEndPtr;
                int delimCount = 0;
                while (secondToLastDelimPtr > currentPosition && delimCount < 2) {
                    if (*--secondToLastDelimPtr == '|')
                        ++delimCount;
                }

                // Read bid and ask values
                const char *tempPtr = secondToLastDelimPtr + 1;
                double currentBid = parsePrice(tempPtr);
                tempPtr = lineEndPtr + 1;
                double currentAsk = parsePrice(tempPtr);

                // Store bid/ask
                bids[securityIndex] = currentBid;
                asks[securityIndex] = currentAsk;

                // If this security index matches target, send response
                if (securityIndex == targetSecurityIndex && currentAsk != 0) {
                    char sendBuffer[128];
                    int bytesToSend = snprintf(
                        sendBuffer, 
                        sizeof(sendBuffer),
                        "CHALLENGE_RESPONSE %d SEC%04d %.10g %.10g Kyle-Parran",
                        challengeId, securityIndex, currentBid, currentAsk
                    );
                    send(tcpSocket, sendBuffer, bytesToSend, MSG_NOSIGNAL);
                    targetSecurityIndex = -1;
                }
            }
            // Check for a challenge ID line
            else if (lineSize >= 13 && !memcmp(currentPosition, "CHALLENGE_ID:", 13)) {
                challengeId = atoi(currentPosition + 13);
                targetSecurityIndex = -1;
            }
            // Check for a target line
            else if (lineSize >= 7 && !memcmp(currentPosition, "TARGET:", 7)) {
                // Format example: TARGET: SECXXXX
                targetSecurityIndex = ti(currentPosition + 10);

                // If we already have a valid ask for this target, respond immediately
                double currentBid = bids[targetSecurityIndex];
                double currentAsk = asks[targetSecurityIndex];
                if (currentAsk != 0) {
                    char sendBuffer[128];
                    int bytesToSend = snprintf(
                        sendBuffer,
                        sizeof(sendBuffer),
                        "CHALLENGE_RESPONSE %d SEC%04d %.10g %.10g Kyle-Parran",
                        challengeId, targetSecurityIndex, currentBid, currentAsk
                    );
                    send(tcpSocket, sendBuffer, bytesToSend, MSG_NOSIGNAL);
                    targetSecurityIndex = -1;
                }
            }

            // Advance to the next line
            currentPosition = lineBreakPtr ? lineBreakPtr + 1 : endPosition;
        }
    }
}