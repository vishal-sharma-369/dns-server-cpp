#include <iostream>
#include <cstring>
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>  // For socket, inet_addr, etc.
#include <unistd.h>     // For close()
#include <cstdint>

#include "dns/dns_message.hpp"

#define BUFFER_SIZE 1024

void parse_messages(char response_msg[], ssize_t bytes_received)
{
    DNS_Message response;

    // Copying the response_msg header to the response.header
    // TODO: Figure out why do we shift the second but, when it is the first bit that is most significant and needs to be shifted
    response.header.ID = std::uint16_t(response_msg[0]) | std::uint16_t(response_msg[1]) << 8;
    response.header.FLAGS = std::uint16_t(response_msg[2]) | std::uint16_t(response_msg[3]) << 8;
    response.header.QDCOUNT = std::uint16_t(response_msg[4]) | std::uint16_t(response_msg[5]) << 8;
    response.header.ANCOUNT = std::uint16_t(response_msg[6]) | std::uint16_t(response_msg[7]) << 8;
    response.header.NSCOUNT = std::uint16_t(response_msg[8]) | std::uint16_t(response_msg[9]) << 8;
    response.header.ARCOUNT = std::uint16_t(response_msg[10]) | std::uint16_t(response_msg[11]) << 8;

    response.header.from_network_order();
    std::cout<<"ID: "<<response.header.ID<<std::endl;
    std::cout<<"FLAGS: "<<response.header.FLAGS<<std::endl;
    std::cout<<"QDCOUNT: "<<response.header.QDCOUNT<<std::endl;
    std::cout<<"ANCOUNT: "<<response.header.ANCOUNT<<std::endl;
    std::cout<<"NSCOUNT: "<<response.header.NSCOUNT<<std::endl;
    std::cout<<"ARCOUNT: "<<response.header.ARCOUNT<<std::endl;

/*
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
*/
}

int main() {
    // 1. Create a UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // 2. Server address setup
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2053);            // Port number
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

// ----------------------------------------------------------------------------------------------------------------------------------
// TODO: I have to replace the format of this message with the dns message format(for now, it's just for testing the proper working of udp server)
    // 3. Message to send to the server
    const char* message = "Hello, server!";
    
    // 4. Send message to the server
    ssize_t sent_bytes = sendto(sockfd, message, strlen(message), 0,
                                (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (sent_bytes < 0) {
        std::cerr << "Error sending packet" << std::endl;
        close(sockfd);
        return -1;
    }
    
    std::cout << "Message sent to server: " << message << std::endl;

    // 5. Receive response from the server
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);
    ssize_t received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr*)&server_addr, &addr_len);
    if (received_bytes < 0) {
        std::cerr << "Error receiving response" << std::endl;
        close(sockfd);
        return -1;
    }
    
    buffer[received_bytes] = '\0'; // Null-terminate the received data

    // 6. Print the response from the server
    // std::cout << "Response from server: " << buffer << std::endl;
    for(int i = 0; i < received_bytes; i++)
    {
        // std::cout<<std::uint8_t(buffer[i]);
        if(buffer[i] >= 97 && buffer[i] <= 122)
        {
            std::cout<<buffer[i];
        }
        else std::cout<<unsigned(buffer[i]);
    }
    std::cout<<std::endl;
    // parse_messages(buffer, received_bytes);

    // 7. Close the socket
    close(sockfd);

    return 0;
}
