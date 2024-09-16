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
void DNS_Answer::write_dns_answer_section(std::vector<std::string> nameDomains, std::vector<std::string> rDataDomains, std::vector<std::string> answerRR)
{
    for(int i = 0; i < nameDomains.size(); i++)
    {
        std::string domain = nameDomains[i];
        this->NAME.push_back((uint8_t) domain.size());
        for(char c: domain)
        {
            this->NAME.push_back(c);
        }
    }

    this->NAME.push_back(0);

    this->TYPE = static_cast<std::uint16_t>(std::stoi(answerRR[1]));
    this->CLASS = static_cast<std::uint16_t>(std::stoi(answerRR[2]));
    this->TTL = static_cast<std::uint32_t>(std::stoul(answerRR[3]));
    this->RDLENGTH = static_cast<std::uint16_t>(std::stoi(answerRR[4]));

    for(int i = 0; i < this->RDLENGTH; i++)
    {
        std::uint8_t ip_segment = static_cast<std::uint8_t>(std::stoi(rDataDomains[i]));
        this->RDATA.push_back(ip_segment);
    }
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
void DNS_Answer::parse_dns_answer_section(std::uint8_t response_msg[], ssize_t bytes_received, std::uint8_t answerSectionStartIndex, std::uint16_t name_length)
{
    this->NAME.resize(name_length);
    std::copy(response_msg + answerSectionStartIndex, response_msg + answerSectionStartIndex + name_length, this->NAME.begin());
    // The extra 10 size is used for adding the TYPE, CLASS, TTL, and RDLENGTH fields of answer section
    std::uint16_t answerSectionStartIndexAfterNAME = answerSectionStartIndex + this->NAME.size();

    this->TYPE = std::uint16_t(response_msg[answerSectionStartIndexAfterNAME]) | std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 1]) << 8;
    this->CLASS = std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 2]) | std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 3]) << 8;
    this->TTL = std::uint32_t(response_msg[answerSectionStartIndexAfterNAME + 4]) | std::uint32_t(response_msg[answerSectionStartIndexAfterNAME + 5]) << 8 | std::uint32_t(response_msg[answerSectionStartIndexAfterNAME + 6]) << 16 | std::uint32_t(response_msg[answerSectionStartIndexAfterNAME + 7]) << 24;
    this->RDLENGTH = std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 8]) | std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 9]) << 8;
    this->RDATA.resize(4);
    std::copy(response_msg + answerSectionStartIndexAfterNAME + 10, response_msg + answerSectionStartIndexAfterNAME + 10 + 4, this->RDATA.begin());
    this->from_network_order();
}