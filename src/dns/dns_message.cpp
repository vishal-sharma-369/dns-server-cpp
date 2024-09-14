#include "dns_message.hpp"

DNS_Message::DNS_Message(){}
void DNS_Message::to_network_order()
{
    header.to_network_order();
    question.to_network_order();
    answer.to_network_order();
}

void DNS_Message::from_network_order()
{
    header.from_network_order();
    question.from_network_order();
    answer.from_network_order();
}

void DNS_Message::write_dns_message()
{
    // Step1: Create the DNS_Header Section
    this->header.ID = 1;
    this->header.FLAGS = 0;
    this->header.FLAGS = (1 << 15);
    this->header.QDCOUNT = 0;
    this->header.ANCOUNT = 1;
    this->header.NSCOUNT = 0;
    this->header.ARCOUNT = 0;

    // Step2: Create the DNS_Question Section
    std::string first_question_domain = "codecrafters";
    this->question.QNAME.push_back((uint8_t)first_question_domain.size());
    for(char c : first_question_domain) {
        this->question.QNAME.push_back(c);
    }

    std::string second_question_domain = "io";
    this->question.QNAME.push_back((uint8_t)second_question_domain.size());
    for(char c : second_question_domain) {
        this->question.QNAME.push_back(c);
    }
    this->question.QNAME.push_back(0);

    this->question.CLASS = 1;
    this->question.TYPE = 1;

    // Step3: Create the DNS_Answer Section
    std::string first_answer_domain = "codecrafters";
    this->answer.NAME.push_back((uint8_t) first_answer_domain.size());
    for(char c: first_answer_domain)
    {
        this->answer.NAME.push_back(c);
    }

    std::string second_answer_domain = "io";
    this->answer.NAME.push_back((uint8_t)second_answer_domain.size());
    for(char c : second_answer_domain)
    {
        this->answer.NAME.push_back(c);
    }
    this->answer.NAME.push_back(0);
    this->answer.TYPE = 1;
    this->answer.CLASS = 1;
    this->answer.TTL = 60;
    this->answer.RDLENGTH = 4;

    std::uint8_t first_ip_segment = 8;
    std::uint8_t second_ip_segment = 8;
    std::uint8_t third_ip_segment = 8;
    std::uint8_t fourth_ip_segment = 8;

    this->answer.RDATA.push_back(first_ip_segment);
    this->answer.RDATA.push_back(second_ip_segment);
    this->answer.RDATA.push_back(third_ip_segment);
    this->answer.RDATA.push_back(fourth_ip_segment);

    this->to_network_order();
}

void DNS_Message::write_dns_message_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend)
{
        // Copying the response header to the response buffer
        std::copy((const char*) &(this->header), (const char*) &(this->header) + sizeof(this->header), responseBuffer);

        // Copying the response question section to the response buffer
        std::uint8_t questionSectionStartIndex = sizeof(this->header);
        std::copy(this->question.QNAME.begin(), this->question.QNAME.end(), responseBuffer + questionSectionStartIndex);
        // The extra 4 size is used for adding the TYPE and CLASS fields of question section(as they are of 2 bytes and character is of 1 byte each, so we need a total of 4 to create response)
        responseBuffer[questionSectionStartIndex + this->question.QNAME.size()] = 0;
        responseBuffer[questionSectionStartIndex + this->question.QNAME.size() + 1] = 1;
        responseBuffer[questionSectionStartIndex + this->question.QNAME.size() + 2] = 0;
        responseBuffer[questionSectionStartIndex + this->question.QNAME.size() + 3] = 1;

        // Copying the response answer section to the response buffer
        std::uint16_t answerSectionStartIndex = sizeof(this->header) + this->question.QNAME.size() +  4;
        std::copy(this->answer.NAME.begin(), this->answer.NAME.end(), responseBuffer + answerSectionStartIndex);
        // The extra 10 size is used for adding the TYPE, CLASS, TTL, and RDLENGTH fields of answer section
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size()] = 0;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 1] = 1;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 2] = 0;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 3] = 1;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 4] = 0;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 5] = 0;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 6] = 0;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 7] = 60;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 8] = 0;
        responseBuffer[answerSectionStartIndex + this->answer.NAME.size() + 9] = 4;
        std::copy(this->answer.RDATA.begin(), this->answer.RDATA.end(), responseBuffer + answerSectionStartIndex + this->answer.NAME.size() + 10);
}