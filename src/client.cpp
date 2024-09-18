#include <iostream>
#include <cstring>
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>  // For socket, inet_addr, etc.
#include <unistd.h>     // For close()
#include <cstdint>

#include "dns/dns_message.hpp"
#include "logs/logging.hpp"

#define BUFFER_SIZE 1024

int setup_socket_client(int &sockfd, struct sockaddr_in &server_addr)
{
    // 1. Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    // 2. Server address setup
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2053);            // Port number
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    // ----------------------------------------------------------------------------------------------------------------------------------
    // 3. Query Message to send to the server
    DNS_Message query;
    query.create_dns_query();
    std::uint16_t bytesToSend = sizeof(query.header);
    for(DNS_Question question : query.questions)
    {
        bytesToSend += question.QNAME.size() + 4;
    }
    for(DNS_Answer answer : query.answers)
    {
        bytesToSend += answer.NAME.size() + 10 + answer.RDATA.size();
    }
    std::uint8_t queryBuffer[bytesToSend];
    std::pair<std::uint16_t, std::uint16_t> querySizeInQueryBuffer =  query.write_dns_message_to_byte_buffer(queryBuffer, bytesToSend);

    // 4. Send message to the server
    ssize_t sent_bytes = sendto(sockfd, queryBuffer, querySizeInQueryBuffer.second, 0,
                                (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (sent_bytes < 0) {
        std::cerr << "Error sending packet" << std::endl;
        close(sockfd);
        return -1;
    }

    write_dns_message_buffer_to_console(queryBuffer, querySizeInQueryBuffer.second, "Message sent to server");
    return 0;    // Indicates successfull socket binding
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    if(setup_socket_client(sockfd, server_addr) == -1)
    {
        return 1;   // Exit code due to some error while sending packets to server
    }

    // Receive response from the server
    std::uint8_t buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);
    ssize_t received_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                      (struct sockaddr*)&server_addr, &addr_len);
    if (received_bytes < 0) {
        std::cerr << "Error receiving response" << std::endl;
        close(sockfd);
        return -1;
    }

    // Print the response from the server
    write_dns_message_buffer_to_console(buffer, received_bytes, "DNS response received");

    // Parse the response
    DNS_Message response;
    response.parse_dns_message(buffer, received_bytes);

    // 7. Close the socket
    close(sockfd);

    return 0;
}
