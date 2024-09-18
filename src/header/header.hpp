#ifndef HEADER_H
#define HEADER_H
#include <cstdint>

class __attribute__((packed)) DNS_Header 
{
public:
    std::uint16_t ID;
    std::uint16_t FLAGS;
    std::uint16_t QDCOUNT;
    std::uint16_t ANCOUNT;
    std::uint16_t NSCOUNT;
    std::uint16_t ARCOUNT;

    DNS_Header();
    void to_network_order();
    void from_network_order();

    //  DNS Header Writer Methods
    void write_dns_header(std::uint16_t ID, std::uint16_t FLAGS, std::uint16_t QDCOUNT, 
std::uint16_t ANCOUNT, std::uint16_t NSCOUNT, std::uint16_t ARCOUNT);
    void write_dns_header_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend);


    //  DNS Header Parser Methods
    void parse_dns_header(std::uint8_t response_msg[], ssize_t bytes_received);
};

#endif