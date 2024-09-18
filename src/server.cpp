#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>

#include "dns/dns_message.hpp"
#include "logs/logging.hpp"

#define BUFFER_SIZE 512

enum FLAGS {
    QR_FLAG = (1 << 15),
    OPCODE_FLAG = (15 << 11),
    RCODE_FLAG = (4),
    RD_FLAG = (1 << 8),
};

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

int setup_resolver(int argc, char** argv, int &resolverSocket, std::string &resolverIP,     struct sockaddr_in& resolverAddress)
{
    std::string resolver, resolver_port;

    for(int i = 1; i < argc; i++)
    {
        if(i + 1 != argc)
        {
            if(strcmp(argv[i], "--resolver") == 0)
            {
                resolver = argv[i+1];
                resolverIP = resolver.substr(0, resolver.find(':'));
                resolver_port = resolver.substr(resolver.find(':') + 1);
                break;
            }
        }
    }

    if(resolverIP.size() > 0) {
        resolverSocket = socket(AF_INET, SOCK_DGRAM, 0);
        if (resolverSocket == -1) {
            std::cerr << "Socket creation failed: " << strerror(errno) << "..." << std::endl;
            return 1;
        }

        resolverAddress = { 
            .sin_family = AF_INET,
            .sin_port = htons(std::stoi(resolver_port)),
        };
        if (inet_pton(AF_INET, resolverIP.c_str(), &resolverAddress.sin_addr) <= 0) {
            std::cerr << "Invalid resolver IP address" << std::endl;
        }
    }
    return 0;
}

int receive_data(int udpSocket, socklen_t& clientAddrLen, struct sockaddr_in& clientAddress, DNS_Message& received_msg)
{
    std::uint8_t buffer[BUFFER_SIZE];
    ssize_t bytesRead = recvfrom(udpSocket, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddrLen);
    if (bytesRead == -1) {
        perror("Error receiving data");
        return 1;
    }

    // Print the received dns message
    write_dns_message_buffer_to_console(buffer, bytesRead, "Received bytes");

    // Parse the response
    received_msg.parse_dns_message(buffer, bytesRead);
    return 0;
}

void create_dummy_response(DNS_Message& response)
{
    
    std::vector<std::vector<std::string>> questions = {
        {"www.upmc.fr", "1", "1"},
        {"www.upmc.fr", "5","1"},
        {"web.upmc.fr", "5","1"}
    };

    std::vector<std::vector<std::string>> answers = {
        {"codecrafters.io", "1", "1", "60", "4", "81.8.8.8"},
        {"google.io", "1", "1", "60", "4", "8.8.80.8"}
    };
    response.write_dns_message(1, 1<<15, 3, 2, 0, 0, questions, answers);
}

void create_query_response(DNS_Message& response, DNS_Message received_msg)
{
    std::vector<std::vector<std::string>> questions;
    for(int i = 0; i < received_msg.questions.size(); i++)
    {
        std::string qname;
        int idx = 0;
        std::uint16_t length = received_msg.questions[i].QNAME[idx];
        while(length != 0)
        {
            for(int j = idx + 1; j < idx + length + 1; j++)
            {
                qname.push_back(received_msg.questions[i].QNAME[j]);
            }
            qname.push_back('.');
            idx += length + 1;
            length = received_msg.questions[i].QNAME[idx];
        }
        qname = qname.substr(0, qname.size()-1);
        questions.push_back({qname,"1","1"});
    }

    // Below loop will never run for answers as client query message's ANCOUNT will always be 0
    std::vector<std::vector<std::string>> answers;
    for(int i = 0; i < received_msg.answers.size(); i++)
    {
        answers.push_back({"","1","1","60","4","8.8.8.8"});
    }

    // I have kept the same qdcount and ancount intentionally as the number of answer records should be equal to the number of question records.
    response.write_dns_message(received_msg.header.ID, received_msg.header.FLAGS, received_msg.header.QDCOUNT, received_msg.header.QDCOUNT,0,0,questions,answers);
}

int query_resolver(int resolver_socket, sockaddr_in resolver_addr, DNS_Message& response, int question_index)
{
    DNS_Message query = response;
    query.header.ANCOUNT = 0;
    query.header.QDCOUNT = htons(1);
    query.header.FLAGS = htons(ntohs(query.header.FLAGS) & (~QR_FLAG));

    // Calculate response size
    std::uint16_t bytesToSend = sizeof(query.header);
    bytesToSend += query.questions[question_index].QNAME.size() + 4;
    std::uint8_t responseBuffer[bytesToSend];
    std::pair<std::uint16_t, std::uint16_t> responseSizeInResponseBuffer =  query.write_dns_message_to_byte_buffer(responseBuffer, bytesToSend);
    
    write_dns_message_buffer_to_console(responseBuffer, responseSizeInResponseBuffer.second, "\nMessage to be sent to google: ");

    // Send response (query in this case)
    if (sendto(resolver_socket, &responseBuffer, responseSizeInResponseBuffer.second , 0, reinterpret_cast<struct sockaddr*>(&resolver_addr), sizeof(resolver_addr)) == -1) {
        perror("Failed to send response");
    }
    else write_message_to_server_log(responseBuffer, responseSizeInResponseBuffer, response);


    int bytesRead;
    std::uint8_t buffer[BUFFER_SIZE];
    socklen_t resolver_addrLen = sizeof(resolver_addr);

    // Receive data
    bytesRead = recvfrom(resolver_socket, buffer, sizeof(buffer), 0, reinterpret_cast<struct sockaddr*>(&resolver_addr), &resolver_addrLen);
    if(bytesRead == -1)
    {
        perror("Error receiving data");
        return 1;
    }

    // Parse the response
    DNS_Message received_msg;
    received_msg.parse_dns_message(buffer, bytesRead);

    if(received_msg.answers[0].CLASS == 1 && received_msg.answers[0].TYPE == 1)
    {
        response.answers.push_back(received_msg.answers[0]);
    }

    return 0;
}


int main(int argc, char **argv) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Disable output buffering
    setbuf(stdout, NULL);

    // Bind the dns server to port 
    int udpSocket;
    if(setup_socket(udpSocket) == 1) return 1;

    // Configure the forwarding server (resolver for dns_queries)
    int resolverSocket;
    std::string resolverIP;
    struct sockaddr_in resolverAddress;
    if(setup_resolver(argc, argv, resolverSocket, resolverIP, resolverAddress) == 1) return 1;

   struct sockaddr_in clientAddress;
   socklen_t clientAddrLen = sizeof(clientAddress);

   while (true) {
        std::cout<<"UDP Server Listening on Port: 2053"<<std::endl;
        DNS_Message received_msg;
        if(receive_data(udpSocket, clientAddrLen, clientAddress, received_msg))
        {
            std::cout<<"Error receiving data"<<std::endl;
            continue;
        }

        // Resolving the queries incase the resolverIP is provided.
        DNS_Message response;
        // -------------------------------------------------------------------
        // -------------------------------------------------------------------
        create_query_response(response, received_msg);
        // Resolving the queries received in the client_dns_query(received_msg)
        int question_index{0};
        while(!resolverIP.empty() && question_index < received_msg.header.QDCOUNT) {
            // query_resolver(resolverSocket, resolverAddress, response, question_index);
            if(query_resolver(resolverSocket, resolverAddress, response, question_index)==1)
            {
                std::cout<<"Unable to resolve the query number: "<<question_index + 1<<std::endl;
            }
            question_index++;
        }

        // Responding with a dummy response in case a resolverIP is not provided.
        if(resolverIP.empty())
        {
            DNS_Message dummy_response;
            create_dummy_response(dummy_response);
            response = dummy_response;
        }

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

    return 0;
}
