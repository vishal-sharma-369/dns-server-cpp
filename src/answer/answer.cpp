#include <iostream>
#include "answer.hpp"
#include "../utilities/utilities.hpp"

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

std::uint16_t DNS_Answer::write_dns_answer_section_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend, std::uint8_t answerSectionStartIndex, std::unordered_map<std::string, std::uint16_t>& domain_name_to_buffer_index_pointer)
{
    int index = 0;
    while(index < this->NAME.size())
    {
        int domain_length = this->NAME[index];
        std::string domain = get_substring(this->NAME, index + 1, index + domain_length);
        if(domain_name_to_buffer_index_pointer.find(domain) == domain_name_to_buffer_index_pointer.end())
        {
            if(domain != "") domain_name_to_buffer_index_pointer[domain] = answerSectionStartIndex;
            responseBuffer[answerSectionStartIndex++] = domain_length;
            std::copy(domain.begin(), domain.end(), responseBuffer + answerSectionStartIndex);
            answerSectionStartIndex += domain.size();
            index += domain_length + 1;
        }
        else 
        {
            std::uint16_t pointerIndex = domain_name_to_buffer_index_pointer[domain];
            responseBuffer[answerSectionStartIndex++] = (0xC0) | ((pointerIndex >> 8) & 0xFF);
            responseBuffer[answerSectionStartIndex++] = (pointerIndex) & 0xFF;
            // We are doing this index += 2 as index can also give us the size of name in response buffer
            // otherwise there is no logical meaning of doing index += 2 as it is not used any further after this step
            index += 2;
            break;
        }
    }

    // The extra 10 size is used for adding the TYPE, CLASS, TTL, and RDLENGTH fields of answer section
    responseBuffer[answerSectionStartIndex] = (this->TYPE >> 8) & 0xFF;
    responseBuffer[answerSectionStartIndex + 1] = this->TYPE & 0xFF;
    responseBuffer[answerSectionStartIndex + 2] = (this->CLASS >> 8) & 0xFF;
    responseBuffer[answerSectionStartIndex + 3] = (this->CLASS) & 0xFF;
    responseBuffer[answerSectionStartIndex + 4] = (this->TTL >> 24) & 0xFF;
    responseBuffer[answerSectionStartIndex + 5] = (this->TTL >> 16) & 0xFF;
    responseBuffer[answerSectionStartIndex + 6] = (this->TTL >> 8) & 0xFF;
    responseBuffer[answerSectionStartIndex + 7] = this->TTL & 0xFF;
    responseBuffer[answerSectionStartIndex + 8] = (this->RDLENGTH >> 8) & 0xFF;
    responseBuffer[answerSectionStartIndex + 9] = (this->RDLENGTH) & 0xFF;
    std::copy(this->RDATA.begin(), this->RDATA.end(), responseBuffer + answerSectionStartIndex + 10);

    return index;
}


//  DNS Answer Parser Methods

void DNS_Answer::parse_dns_answer_section(std::uint8_t response_msg[], ssize_t bytes_received, std::uint8_t answerSectionStartIndex, std::uint16_t name_length)
{
    // this->NAME.resize(name_length);
    // std::copy(response_msg + answerSectionStartIndex, response_msg + answerSectionStartIndex + name_length, this->NAME.begin());
    std::uint8_t originalAnswerSectionStartingIndex = answerSectionStartIndex;
    std::uint8_t length = response_msg[answerSectionStartIndex];
    std::uint16_t name_size = 1;
    bool is_compressed = false;
    while(length != 0)
    {
        if(isPointer(length))
        {
            uint16_t offset = ((length & 0x3F) << 8) | static_cast<uint8_t>((response_msg[answerSectionStartIndex + 1]));
            answerSectionStartIndex = offset;
            name_size += 2;
            is_compressed = true;
        }
        else 
        {
            for(int i = answerSectionStartIndex; i < answerSectionStartIndex + length + 1; i++)
            {
                this->NAME.push_back(response_msg[i]);
            }
            answerSectionStartIndex += length + 1;
            if(!is_compressed) name_size += length + 1;
        }

        length = response_msg[answerSectionStartIndex];
    }

    if(is_compressed) name_size -= 1;
    this->NAME.push_back(0);

    // The extra 10 size is used for adding the TYPE, CLASS, TTL, and RDLENGTH fields of answer section
    std::uint16_t answerSectionStartIndexAfterNAME = originalAnswerSectionStartingIndex + name_size;

    this->TYPE = std::uint16_t(response_msg[answerSectionStartIndexAfterNAME]) | std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 1]) << 8;
    this->CLASS = std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 2]) | std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 3]) << 8;
    this->TTL = std::uint32_t(response_msg[answerSectionStartIndexAfterNAME + 4]) | std::uint32_t(response_msg[answerSectionStartIndexAfterNAME + 5]) << 8 | std::uint32_t(response_msg[answerSectionStartIndexAfterNAME + 6]) << 16 | std::uint32_t(response_msg[answerSectionStartIndexAfterNAME + 7]) << 24;
    this->RDLENGTH = std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 8]) | std::uint16_t(response_msg[answerSectionStartIndexAfterNAME + 9]) << 8;
    this->from_network_order();
    this->RDATA.resize(this->RDLENGTH);
    std::copy(response_msg + answerSectionStartIndexAfterNAME + 10, response_msg + answerSectionStartIndexAfterNAME + 10 + this->RDLENGTH, this->RDATA.begin());
}
