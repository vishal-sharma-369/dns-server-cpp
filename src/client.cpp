#include <iostream>
#include <cstring>
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h>  // For socket, inet_addr, etc.
#include <unistd.h>     // For close()

#define BUFFER_SIZE 1024

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
    std::cout << "Response from server: " << buffer << std::endl;

    // 7. Close the socket
    close(sockfd);

    return 0;
}
