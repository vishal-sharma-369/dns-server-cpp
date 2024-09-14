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


/* 
-----------------------------------------------------------------------------------
-> DNS Message Parser Methods
-----------------------------------------------------------------------------------
*/

// TODO: Shift this to header handler module
void parse_header(char response_msg[], ssize_t bytes_received, DNS_Message& response)
{
    // Copying the response_msg header to the response.header
    // TODO: Figure out why do we shift the second but, when it is the first bit that is most significant and needs to be shifted
    response.header.ID = std::uint16_t(response_msg[0]) | std::uint16_t(response_msg[1]) << 8;
    response.header.FLAGS = std::uint16_t(response_msg[2]) | std::uint16_t(response_msg[3]) << 8;
    response.header.QDCOUNT = std::uint16_t(response_msg[4]) | std::uint16_t(response_msg[5]) << 8;
    response.header.ANCOUNT = std::uint16_t(response_msg[6]) | std::uint16_t(response_msg[7]) << 8;
    response.header.NSCOUNT = std::uint16_t(response_msg[8]) | std::uint16_t(response_msg[9]) << 8;
    response.header.ARCOUNT = std::uint16_t(response_msg[10]) | std::uint16_t(response_msg[11]) << 8;

    response.header.from_network_order();
}

void DNS_Message::parse_dns_message(std::uint8_t response_msg[], ssize_t bytes_received)
{
    // parse_header(response_msg, bytes_received, response);

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
