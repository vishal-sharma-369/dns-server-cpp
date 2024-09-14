#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "dns/dns_message.hpp"

void write_message(DNS_Message& response_msg)
{
    // Step1: Create the DNS_Header Section
    response_msg.header.ID = 1234;
    response_msg.header.FLAGS = 0;
    response_msg.header.FLAGS = (1 << 15);
    response_msg.header.QDCOUNT = 0;
    response_msg.header.ANCOUNT = 1;
    response_msg.header.NSCOUNT = 0;
    response_msg.header.ARCOUNT = 0;

    // Step2: Create the DNS_Question Section
    std::string first_question_domain = "codecrafters";
    response_msg.question.QNAME.push_back((uint8_t)first_question_domain.size());
    for(char c : first_question_domain) {
        response_msg.question.QNAME.push_back(c);
    }

    std::string second_question_domain = "io";
    response_msg.question.QNAME.push_back((uint8_t)second_question_domain.size());
    for(char c : second_question_domain) {
        response_msg.question.QNAME.push_back(c);
    }
    response_msg.question.QNAME.push_back(0);

    response_msg.question.CLASS = 1;
    response_msg.question.TYPE = 1;

    // Step3: Create the DNS_Answer Section
    std::string first_answer_domain = "codecrafters";
    response_msg.answer.NAME.push_back((uint8_t) first_answer_domain.size());
    for(char c: first_answer_domain)
    {
        response_msg.answer.NAME.push_back(c);
    }

    std::string second_answer_domain = "io";
    response_msg.answer.NAME.push_back((uint8_t)second_answer_domain.size());
    for(char c : second_answer_domain)
    {
        response_msg.answer.NAME.push_back(c);
    }
    response_msg.answer.NAME.push_back(0);
    response_msg.answer.TYPE = 1;
    response_msg.answer.CLASS = 1;
    response_msg.answer.TTL = 60;
    response_msg.answer.RDLENGTH = 4;

    std::uint8_t first_ip_segment = 8;
    std::uint8_t second_ip_segment = 8;
    std::uint8_t third_ip_segment = 8;
    std::uint8_t fourth_ip_segment = 8;

    response_msg.answer.RDATA.push_back(first_ip_segment);
    response_msg.answer.RDATA.push_back(second_ip_segment);
    response_msg.answer.RDATA.push_back(third_ip_segment);
    response_msg.answer.RDATA.push_back(fourth_ip_segment);

    response_msg.to_network_order();
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

        DNS_Message response;
        write_message(response);

        char responseBuffer[sizeof(response.header) + (response.question.QNAME.size()  + 4) + (response.answer.NAME.size() + 10 + response.answer.RDATA.size())];

        // Copying the response header to the response buffer
        std::copy((const char*) &response.header, (const char*) &response.header + sizeof(response.header), responseBuffer);

        // Copying the response question section to the response buffer
        std::uint8_t questionSectionStartIndex = sizeof(response.header);
        std::copy(response.question.QNAME.begin(), response.question.QNAME.end(), responseBuffer + questionSectionStartIndex);
        // The extra 4 size is used for adding the TYPE and CLASS fields of question section(as they are of 2 bytes and character is of 1 byte each, so we need a total of 4 to create response)
        responseBuffer[questionSectionStartIndex + response.question.QNAME.size()] = 0;
        responseBuffer[questionSectionStartIndex + response.question.QNAME.size() + 1] = 1;
        responseBuffer[questionSectionStartIndex + response.question.QNAME.size() + 2] = 0;
        responseBuffer[questionSectionStartIndex + response.question.QNAME.size() + 3] = 1;

        // Copying the response answer section to the response buffer
        std::uint16_t answerSectionStartIndex = sizeof(response.header) + response.question.QNAME.size() +  4;
        std::copy(response.answer.NAME.begin(), response.answer.NAME.end(), responseBuffer + answerSectionStartIndex);
        // The extra 10 size is used for adding the TYPE, CLASS, TTL, and RDLENGTH fields of answer section
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size()] = 0;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 1] = 1;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 2] = 0;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 3] = 1;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 4] = 0;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 5] = 0;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 6] = 0;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 7] = 60;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 8] = 0;
        responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 9] = 4;
        std::copy(response.answer.RDATA.begin(), response.answer.RDATA.end(), responseBuffer + answerSectionStartIndex + response.answer.NAME.size() + 10);
        std::cout<<"Created the header, question, and answer section response"<<std::endl;

        std::cout<<"Buffer size: "<<sizeof(responseBuffer)<<std::endl;
        std::cout<<"Header size: "<<sizeof(response.header)<<std::endl;
        std::cout<<"Question QNAME size: "<<response.question.QNAME.size()<<std::endl;
        std::cout<<"Question Section size: "<<response.question.QNAME.size() + 4<<std::endl;
        std::cout<<"Answer NAME size: "<<response.answer.NAME.size() << std::endl;
        std::cout<<"Answer RDATA size: "<<response.answer.RDATA.size() << std::endl;
        std::cout<<"Answer Section size: "<<response.answer.NAME.size() + 10 + response.answer.RDATA.size() << std::endl;
        
        std::cout<<std::string(responseBuffer, responseBuffer + sizeof(responseBuffer))<<std::endl;
        // Send response
        if (sendto(udpSocket, responseBuffer, sizeof(responseBuffer), 0, reinterpret_cast<struct sockaddr*>(&clientAddress), sizeof(clientAddress)) == -1) {
            perror("Failed to send response");
        }
   }

   close(udpSocket);

    return 0;;
}
