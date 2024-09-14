#include <iostream>
#include <arpa/inet.h>
#include "header.hpp"

DNS_Header::DNS_Header(){}
void DNS_Header::to_network_order()
{
    ID = htons(ID);
    FLAGS = htons(FLAGS);
    QDCOUNT = htons(QDCOUNT);
    ANCOUNT = htons(ANCOUNT);
    NSCOUNT = htons(NSCOUNT);
    ARCOUNT = htons(ARCOUNT);
}

void DNS_Header::from_network_order()
{
    ID = ntohs(ID);
    FLAGS = ntohs(FLAGS);
    QDCOUNT = ntohs(QDCOUNT);
    ANCOUNT = ntohs(ANCOUNT);
    NSCOUNT = ntohs(NSCOUNT);
    ARCOUNT = ntohs(ARCOUNT);
}

//  DNS Header Writer Methods
void DNS_Header::write_dns_header()
{
    this->ID = 1;
    this->FLAGS = 0;
    this->FLAGS = (1 << 15);
    this->QDCOUNT = 0;
    this->ANCOUNT = 1;
    this->NSCOUNT = 0;
    this->ARCOUNT = 0;
}

void DNS_Header::write_dns_header_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend)
{
    std::copy((const char*) this, (const char*) this + sizeof(*this), responseBuffer);
}

//  DNS Header Parser Methods
void DNS_Header::parse_dns_header(std::uint8_t response_msg[], ssize_t bytes_received)
{
    // Copying the response_msg header to the response.header
    // TODO: Figure out why do we shift the second but, when it is the first bit that is most significant and needs to be shifted
    this->ID = std::uint16_t(response_msg[0]) | std::uint16_t(response_msg[1]) << 8;
    this->FLAGS = std::uint16_t(response_msg[2]) | std::uint16_t(response_msg[3]) << 8;
    this->QDCOUNT = std::uint16_t(response_msg[4]) | std::uint16_t(response_msg[5]) << 8;
    this->ANCOUNT = std::uint16_t(response_msg[6]) | std::uint16_t(response_msg[7]) << 8;
    this->NSCOUNT = std::uint16_t(response_msg[8]) | std::uint16_t(response_msg[9]) << 8;
    this->ARCOUNT = std::uint16_t(response_msg[10]) | std::uint16_t(response_msg[11]) << 8;

    this->from_network_order();
}
