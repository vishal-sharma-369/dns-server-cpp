#ifndef QUESTION_H
#define QUESTION_H
#include <iostream>
#include <cstdint>
#include <vector>

class DNS_Question
{
    public:
        std::vector<std::uint8_t> QNAME;
        std::uint16_t TYPE;
        std::uint16_t CLASS;
    
    DNS_Question();
    void to_network_order();
    void from_network_order();

    //  DNS Question Writer Methods
    void write_dns_question_section();
    void write_dns_question_section_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend, std::uint8_t questionSectionStartIndex);


    //  DNS Question Parser Methods
    void parse_dns_question_section(std::uint8_t response_msg[], ssize_t bytes_received, std::uint8_t questionSectionStartIndex, std::uint16_t qname_length);
};

#endif