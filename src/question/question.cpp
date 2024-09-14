#include <arpa/inet.h>
#include "question.hpp"

DNS_Question::DNS_Question(){}
void DNS_Question::to_network_order()
{
    TYPE = htons(TYPE);
    CLASS = htons(CLASS);
}

void DNS_Question::from_network_order()
{
    TYPE = ntohs(TYPE);
    CLASS = ntohs(CLASS);
}

//  DNS Question Writer Methods
void DNS_Question::write_dns_question_section()
{
    std::string first_question_domain = "codecrafters";
    this->QNAME.push_back((uint8_t)first_question_domain.size());
    for(char c : first_question_domain) {
        this->QNAME.push_back(c);
    }

    std::string second_question_domain = "io";
    this->QNAME.push_back((uint8_t)second_question_domain.size());
    for(char c : second_question_domain) {
        this->QNAME.push_back(c);
    }
    this->QNAME.push_back(0);

    this->CLASS = 1;
    this->TYPE = 1;
}

void DNS_Question::write_dns_question_section_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend, std::uint8_t questionSectionStartIndex)
{
    std::copy(this->QNAME.begin(), this->QNAME.end(), responseBuffer + questionSectionStartIndex);
    // The extra 4 size is used for adding the TYPE and CLASS fields of question section(as they are of 2 bytes and character is of 1 byte each, so we need a total of 4 to create response)
    responseBuffer[questionSectionStartIndex + this->QNAME.size()] = 0;
    responseBuffer[questionSectionStartIndex + this->QNAME.size() + 1] = 1;
    responseBuffer[questionSectionStartIndex + this->QNAME.size() + 2] = 0;
    responseBuffer[questionSectionStartIndex + this->QNAME.size() + 3] = 1;
}

//  DNS Question Parser Methods
void DNS_Question::parse_dns_question_section(std::uint8_t response_msg[], ssize_t bytes_received, std::uint8_t questionSectionStartIndex, std::uint16_t qname_length)
{
    this->QNAME.resize(qname_length);
    std::copy(response_msg + questionSectionStartIndex, response_msg + questionSectionStartIndex + qname_length, this->QNAME.begin());
    // The extra 4 size is used for adding the TYPE and CLASS fields of question section(as they are of 2 bytes and character is of 1 byte each, so we need a total of 4 to create response)
    std::uint16_t questionSectionStartIndexAfterQNAME = questionSectionStartIndex + this->QNAME.size();
    this->TYPE = std::uint16_t(response_msg[questionSectionStartIndexAfterQNAME]) | std::uint16_t(response_msg[questionSectionStartIndexAfterQNAME + 1]) << 8;
    this->CLASS = std::uint16_t(response_msg[questionSectionStartIndexAfterQNAME + 2]) | std::uint16_t(response_msg[questionSectionStartIndexAfterQNAME + 3]) << 8;
    this->from_network_order();
}
