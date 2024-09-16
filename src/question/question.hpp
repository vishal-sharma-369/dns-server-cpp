#ifndef QUESTION_H
#define QUESTION_H
#include <iostream>
#include <cstdint>
#include <vector>
#include <unordered_map>

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
    std::string get_substring(std::vector<std::uint8_t> str, std::uint16_t start, std::uint16_t end);
    void write_dns_question_section(std::vector<std::string> domains, std::uint16_t type, std::uint16_t clss);
    std::uint16_t write_dns_question_section_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend, std::uint8_t questionSectionStartIndex, std::unordered_map<std::string, std::uint16_t> &domain_name_to_buffer_index_pointer);


    //  DNS Question Parser Methods
    bool isPointer(std::uint8_t x);
    void parse_dns_question_section(std::uint8_t response_msg[], ssize_t bytes_received, std::uint8_t questionSectionStartIndex, std::uint16_t qname_length);
};

#endif