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
};

#endif