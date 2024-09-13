#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "dns/dns_message.hpp"

std::string write_message()
{
    DNS_Message msg;
    msg.ID = 1234;
    msg.FLAGS = (1 << 15);
    msg.QDCOUNT = 0;
    msg.ANCOUNT = 0;
    msg.NSCOUNT = 0;
    msg.ARCOUNT = 0;

    msg.to_network_order();
    std::string header = std::to_string(msg.ID) + std::to_string(msg.FLAGS) +  std::to_string(msg.QDCOUNT) + std::to_string(msg.ANCOUNT) + std::to_string(msg.NSCOUNT) + std::to_string(msg.ARCOUNT);
    return header;
}

int main() {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Disable output buffering
    setbuf(stdout, NULL);

   int udpSocket;
   struct sockaddr_in clientAddress;

   udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
   if (udpSocket == -1) {
       std::cerr << "Socket creation failed: " << strerror(errno) << "..." << std::endl;
       return 1;
   }

   int reuse = 1;
   if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
       std::cerr << "SO_REUSEPORT failed: " << strerror(errno) << std::endl;
       return 1;
   }

   sockaddr_in serv_addr = { .sin_family = AF_INET,
                             .sin_port = htons(2053),
                             .sin_addr = { htonl(INADDR_ANY) },
                           };

   if (bind(udpSocket, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) != 0) {
       std::cerr << "Bind failed: " << strerror(errno) << std::endl;
       return 1;
   }

   int bytesRead;
   char buffer[512];
   socklen_t clientAddrLen = sizeof(clientAddress);

   while (true) {
        // Receive data
        std::cout<<"UDP Server Listening on Port: 2053"<<std::endl;
        bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddrLen);
        if (bytesRead == -1) {
            perror("Error receiving data");
            break;
        }

        buffer[bytesRead] = '\0';
        std::cout << "Received " << bytesRead << " bytes: " << buffer << std::endl;

        // Create an empty response
        //    char response[1] = { '\0' };
        std::string response = write_message();

        std::cout<< response << std::endl;
        // Send response
        if (sendto(udpSocket, response.c_str(), sizeof(response), 0, reinterpret_cast<struct sockaddr*>(&clientAddress), sizeof(clientAddress)) == -1) {
            perror("Failed to send response");
        }
   }

   close(udpSocket);

    return 0;
}
