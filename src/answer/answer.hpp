#ifndef ANSWER_H
#define ANSWER_H
#include <arpa/inet.h>
#include <vector>
#include <cstdint>

class DNS_Answer
{
public:
    std::vector<std::uint8_t> NAME;
    std::uint16_t TYPE;
    std::uint16_t CLASS;
    std::uint32_t TTL;
    std::uint16_t RDLENGTH;
    std::vector<std::uint8_t> RDATA;        // can also use std::uint32_t as there are always 4 segments of 8 bit octect

    DNS_Answer();
    void to_network_order();
    void from_network_order();

    //  DNS Answer Writer Methods
    void write_dns_answer_section();
    void write_dns_answer_section_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend, std::uint8_t answerSectionStartIndex);


    //  DNS Answer Parser Methods
    void parse_dns_answer_section(std::uint8_t response_msg[], ssize_t bytes_received);
};

#endif