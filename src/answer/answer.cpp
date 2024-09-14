#include <iostream>
#include "answer.hpp"

DNS_Answer::DNS_Answer(){}

void DNS_Answer::to_network_order()
{
    TYPE = htons(TYPE);
    CLASS = htons(CLASS);
    TTL = htonl(TTL);
    RDLENGTH = htons(RDLENGTH);
}

void DNS_Answer::from_network_order()
{
    TYPE = ntohs(TYPE);        // Convert 16-bit from network to host byte order
    CLASS = ntohs(CLASS);      // Convert 16-bit from network to host byte order
    TTL = ntohl(TTL);          // Convert 32-bit from network to host byte order
    RDLENGTH = ntohs(RDLENGTH);// Convert 16-bit from network to host byte order
}

//  DNS Answer Writer Methods
void DNS_Answer::write_dns_answer_section()
{
    std::string first_answer_domain = "codecrafters";
    this->NAME.push_back((uint8_t) first_answer_domain.size());
    for(char c: first_answer_domain)
    {
        this->NAME.push_back(c);
    }

    std::string second_answer_domain = "io";
    this->NAME.push_back((uint8_t)second_answer_domain.size());
    for(char c : second_answer_domain)
    {
        this->NAME.push_back(c);
    }
    this->NAME.push_back(0);
    this->TYPE = 1;
    this->CLASS = 1;
    this->TTL = 60;
    this->RDLENGTH = 4;

    std::uint8_t first_ip_segment = 8;
    std::uint8_t second_ip_segment = 8;
    std::uint8_t third_ip_segment = 8;
    std::uint8_t fourth_ip_segment = 8;

    this->RDATA.push_back(first_ip_segment);
    this->RDATA.push_back(second_ip_segment);
    this->RDATA.push_back(third_ip_segment);
    this->RDATA.push_back(fourth_ip_segment);
}

void DNS_Answer::write_dns_answer_section_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend, std::uint8_t answerSectionStartIndex)
{
    std::copy(this->NAME.begin(), this->NAME.end(), responseBuffer + answerSectionStartIndex);
    // The extra 10 size is used for adding the TYPE, CLASS, TTL, and RDLENGTH fields of answer section
    responseBuffer[answerSectionStartIndex + this->NAME.size()] = 0;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 1] = 1;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 2] = 0;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 3] = 1;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 4] = 0;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 5] = 0;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 6] = 0;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 7] = 60;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 8] = 0;
    responseBuffer[answerSectionStartIndex + this->NAME.size() + 9] = 4;
    std::copy(this->RDATA.begin(), this->RDATA.end(), responseBuffer + answerSectionStartIndex + this->NAME.size() + 10);
}


//  DNS Answer Parser Methods
void DNS_Answer::parse_dns_answer_section(std::uint8_t response_msg[], ssize_t bytes_received)
{

}