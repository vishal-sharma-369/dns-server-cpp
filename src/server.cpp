#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>

#include "dns/dns_message.hpp"
#include "logs/logging.hpp"

int setup_socket(int &udpSocket)
{
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
   return 0;    // Indicates successfull socket binding
}

int setup_resolver(int &resolverSocket, std::string resolver_ip, std::string resolver_port,struct sockaddr_in& resolverAddress)
{
    int resolver_socket;
    if(resolver_ip.size() > 0) {
        resolver_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (resolver_socket == -1) {
            std::cerr << "Socket creation failed: " << strerror(errno) << "..." << std::endl;
            return 1;
        }

        resolverAddress = { 
            .sin_family = AF_INET,
            .sin_port = htons(std::stoi(resolver_port)),
        };
        if (inet_pton(AF_INET, resolver_ip.c_str(), &resolverAddress.sin_addr) <= 0) {
            std::cerr << "Invalid resolver IP address" << std::endl;
        }
    }
    return 0;
}


int main(int argc, char **argv) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Disable output buffering
    setbuf(stdout, NULL);

   int udpSocket;
   struct sockaddr_in clientAddress;

    if(setup_socket(udpSocket) == 1) return 1;

    std::string resolver, resolver_ip, resolver_port;
    for(int i = 1; i < argc; i++)
    {
        if(i + 1 != argc)
        {
            if(strcmp(argv[i], "--resolver") == 0)
            {
                resolver = argv[i+1];
                resolver_ip = resolver.substr(0, resolver.find(':'));
                resolver_port = resolver.substr(resolver.find(':') + 1);
                break;
            }
        }
    }

    int resolverSocket;
    struct sockaddr_in resolverAddress;
    if(setup_resolver(resolverSocket, resolver_ip, resolver_port, resolverAddress) == 1) return 1;
   
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

        DNS_Message response;
        response.write_dns_message();

        std::uint16_t bytesToSend = sizeof(response.header);
        for(DNS_Question question : response.questions)
        {
            bytesToSend += question.QNAME.size() + 4;
        }
        for(DNS_Answer answer : response.answers)
        {
            bytesToSend += answer.NAME.size() + 10 + answer.RDATA.size();
        }
        std::uint8_t responseBuffer[bytesToSend];
        std::pair<std::uint16_t, std::uint16_t> responseSizeInResponseBuffer =  response.write_dns_message_to_byte_buffer(responseBuffer, bytesToSend);

        // Send response
        if (sendto(udpSocket, responseBuffer, responseSizeInResponseBuffer.second , 0, reinterpret_cast<struct sockaddr*>(&clientAddress), sizeof(clientAddress)) == -1) {
            perror("Failed to send response");
        }
        else write_message_to_server_log(responseBuffer, responseSizeInResponseBuffer, response);
   }

   close(udpSocket);

    return 0;;
}
