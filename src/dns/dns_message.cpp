#include <unistd.h>
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

/* 
-----------------------------------------------------------------------------------
-> DNS Response Writer Methods
-----------------------------------------------------------------------------------
*/

void DNS_Message::write_dns_message()
{
    // Step1: Create the DNS_Header Section
    this->header.write_dns_header();

    // Step2: Create the DNS_Question Section
    this->question.write_dns_question_section();

    // Step3: Create the DNS_Answer Section
    this->answer.write_dns_answer_section();

    this->to_network_order();
}

void DNS_Message::write_dns_message_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend)
{
        // Copying the response header to the response buffer
        this->header.write_dns_header_to_byte_buffer(responseBuffer, bytesToSend);

        // Copying the response question section to the response buffer
        std::uint8_t questionSectionStartIndex = sizeof(this->header);
        this->question.write_dns_question_section_to_byte_buffer(responseBuffer, bytesToSend, questionSectionStartIndex);

        // Copying the response answer section to the response buffer
        std::uint16_t answerSectionStartIndex = sizeof(this->header) + this->question.QNAME.size() +  4;
        this->answer.write_dns_answer_section_to_byte_buffer(responseBuffer, bytesToSend, answerSectionStartIndex);
}


/* 
-----------------------------------------------------------------------------------
-> DNS Message Parser Methods
-----------------------------------------------------------------------------------
*/

void DNS_Message::parse_dns_message(std::uint8_t response_msg[], ssize_t bytes_received)
{
    this->header.parse_dns_header(response_msg, bytes_received);

    std::cout<<"ID: "<< this->header.ID <<std::endl;
    std::cout<<"FLAGS: "<< this->header.FLAGS <<std::endl;
    std::cout<<"QDCOUNT: "<< this->header.QDCOUNT <<std::endl;
    std::cout<<"ANCOUNT: "<< this->header.ANCOUNT <<std::endl;
    std::cout<<"NSCOUNT: "<< this->header.NSCOUNT <<std::endl;
    std::cout<<"ARCOUNT: "<< this->header.ARCOUNT <<std::endl;

/*
    // Copying the response question section to the response buffer
    std::uint8_t questionSectionStartIndex = sizeof(response.header);
    std::copy(response.question.QNAME.begin(), response.question.QNAME.end(), responseBuffer + questionSectionStartIndex);
    // The extra 4 size is used for adding the TYPE and CLASS fields of question section(as they are of 2 bytes and character is of 1 byte each, so we need a total of 4 to create response)
    responseBuffer[questionSectionStartIndex + response.question.QNAME.size()] = 0;
    responseBuffer[questionSectionStartIndex + response.question.QNAME.size() + 1] = 1;
    responseBuffer[questionSectionStartIndex + response.question.QNAME.size() + 2] = 0;
    responseBuffer[questionSectionStartIndex + response.question.QNAME.size() + 3] = 1;

    // Copying the response answer section to the response buffer
    std::uint16_t answerSectionStartIndex = sizeof(response.header) + response.question.QNAME.size() +  4;
    std::copy(response.answer.NAME.begin(), response.answer.NAME.end(), responseBuffer + answerSectionStartIndex);
    // The extra 10 size is used for adding the TYPE, CLASS, TTL, and RDLENGTH fields of answer section
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size()] = 0;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 1] = 1;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 2] = 0;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 3] = 1;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 4] = 0;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 5] = 0;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 6] = 0;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 7] = 60;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 8] = 0;
    responseBuffer[answerSectionStartIndex + response.answer.NAME.size() + 9] = 4;
    std::copy(response.answer.RDATA.begin(), response.answer.RDATA.end(), responseBuffer + answerSectionStartIndex + response.answer.NAME.size() + 10);
    std::cout<<"Created the header, question, and answer section response"<<std::endl;

    std::cout<<"Buffer size: "<<sizeof(responseBuffer)<<std::endl;
    std::cout<<"Header size: "<<sizeof(response.header)<<std::endl;
    std::cout<<"Question QNAME size: "<<response.question.QNAME.size()<<std::endl;
    std::cout<<"Question Section size: "<<response.question.QNAME.size() + 4<<std::endl;
    std::cout<<"Answer NAME size: "<<response.answer.NAME.size() << std::endl;
    std::cout<<"Answer RDATA size: "<<response.answer.RDATA.size() << std::endl;
    std::cout<<"Answer Section size: "<<response.answer.NAME.size() + 10 + response.answer.RDATA.size() << std::endl;
*/
}
