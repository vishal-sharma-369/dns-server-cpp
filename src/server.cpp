#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "dns/dns_message.hpp"

void write_message(DNS_Message& response)
{
    // std::string header = std::to_string(msg.ID) + std::to_string(msg.FLAGS) +  std::to_string(msg.QDCOUNT) + std::to_string(msg.ANCOUNT) + std::to_string(msg.NSCOUNT) + std::to_string(msg.ARCOUNT);
    // return header;

    DNS_Message msg;

    // Step1: Create the DNS_Header Section
    msg.header.ID = 1234;
    msg.header.FLAGS = (1 << 15);
    msg.header.QDCOUNT = 0;
    msg.header.ANCOUNT = 0;
    msg.header.NSCOUNT = 0;
    msg.header.ARCOUNT = 0;
    // msg.header.to_network_order();

    // Step2: Create the DNS_Question Section
    // msg.question.QNAME = "\\x0ccodecrafters\\x02io";
    // msg.question.TYPE = 1;
    // msg.question.CLASS = 1;
    // msg.question.to_network_order();

    // std::string header_section = std::to_string(msg.header.ID) + std::to_string(msg.header.FLAGS) +  std::to_string(msg.header.QDCOUNT) + std::to_string(msg.header.ANCOUNT) + std::to_string(msg.header.NSCOUNT) + std::to_string(msg.header.ARCOUNT);
    // std::string question_section = msg.question.QNAME + std::to_string(msg.question.TYPE) + std::to_string(msg.question.CLASS);

    // std::string response = header_section + question_section;
    // return response;


    // Step2: Create the DNS_Question Section
    std::string first_domain = "codecrafters";
    msg.question.QNAME.push_back((uint8_t)first_domain.size());
    for(char c : first_domain) {
        msg.question.QNAME.push_back(c);
    }

    std::string second_domain = "io";
    msg.question.QNAME.push_back((uint8_t)second_domain.size());
    for(char c : second_domain) {
        msg.question.QNAME.push_back(c);
    }
    msg.question.QNAME.push_back(0);

    msg.question.CLASS = 1;
    msg.question.TYPE = 1;

    msg.to_network_order();
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
        DNS_Message response;
        write_message(response);

        char responseBuffer[sizeof(response.header) + response.question.QNAME.size()  + 4];
        std::copy((const char*) &response.header, (const char*) &response.header + sizeof(response.header), responseBuffer);
        std::copy(response.question.QNAME.begin(), response.question.QNAME.end(), responseBuffer + sizeof(response.header));
        // The extra 4 size is used for adding the TYPE and CLASS fields of header section(as they are of 2 bytes and character is of 1 byte each, so we need a total of 4 to create response)
        responseBuffer[sizeof(response.header) + response.question.QNAME.size()] = 0;
        responseBuffer[sizeof(response.header) + response.question.QNAME.size() + 1] = 1;
        responseBuffer[sizeof(response.header) + response.question.QNAME.size() + 2] = 0;
        responseBuffer[sizeof(response.header) + response.question.QNAME.size() + 3] = 1;


        // Send response
        if (sendto(udpSocket, responseBuffer, sizeof(responseBuffer), 0, reinterpret_cast<struct sockaddr*>(&clientAddress), sizeof(clientAddress)) == -1) {
            perror("Failed to send response");
        }
   }

   close(udpSocket);

    return 0;
}
