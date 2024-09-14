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

/*
* This function computes the length of the QNAME field of question section
* This function computes the length of the NAME field of answer section
*/

std::uint16_t compute_name_length(std::uint8_t response_msg[], ssize_t bytes_received, std::uint8_t start_index)
{
    std::uint16_t length = 1;
    std::uint16_t domain_name_length = response_msg[start_index];
    while(domain_name_length)
    {
        length += domain_name_length + 1;
        start_index = start_index + domain_name_length + 1;
        domain_name_length = response_msg[start_index];
    }
    return length;
}

void DNS_Message::parse_dns_message(std::uint8_t response_msg[], ssize_t bytes_received)
{
    // Parse dns header
    this->header.parse_dns_header(response_msg, bytes_received);

    std::cout<<"Displaying Header Details Received: "<<std::endl;
    std::cout<<"ID: "<< this->header.ID <<std::endl;
    std::cout<<"FLAGS: "<< this->header.FLAGS <<std::endl;
    std::cout<<"QDCOUNT: "<< this->header.QDCOUNT <<std::endl;
    std::cout<<"ANCOUNT: "<< this->header.ANCOUNT <<std::endl;
    std::cout<<"NSCOUNT: "<< this->header.NSCOUNT <<std::endl;
    std::cout<<"ARCOUNT: "<< this->header.ARCOUNT <<std::endl;

    // Parse dns question section
    std::uint8_t questionSectionStartIndex = sizeof(this->header);
    std::uint16_t qname_length = compute_name_length(response_msg, bytes_received, questionSectionStartIndex);
    this->question.parse_dns_question_section(response_msg, bytes_received, questionSectionStartIndex, qname_length);

    std::cout<<"\n\nDisplaying Question Section Details Received: "<<std::endl;
    std::cout<<"QNAME: ";
    for(std::uint8_t b : this->question.QNAME)
    {
        if(b >= 97 && b <= 122)
        {
            std::cout<<b<<" ";
        }
        else std::cout<<unsigned(b)<<" ";
    }
    std::cout<<std::endl;
    std::cout<<"TYPE: "<< this->question.TYPE <<std::endl;
    std::cout<<"CLASS: "<< this->question.CLASS <<std::endl;

    // Parse dns answer section
    std::uint16_t answerSectionStartIndex = sizeof(this->header) + this->question.QNAME.size() +  4;
    std::uint16_t name_length = compute_name_length(response_msg, bytes_received, answerSectionStartIndex);
    this->answer.parse_dns_answer_section(response_msg, bytes_received, answerSectionStartIndex, name_length);

    std::cout<<"\n\nDisplaying Answer Section Details Received: "<<std::endl;
    std::cout<<"NAME: ";
    for(std::uint8_t b : this->answer.NAME)
    {
        if(b >= 97 && b <= 122)
        {
            std::cout<<b<<" ";
        }
        else std::cout<<unsigned(b)<<" ";
    }
    std::cout<<std::endl;
    std::cout<<"TYPE: "<< this->answer.TYPE <<std::endl;
    std::cout<<"CLASS: "<< this->answer.CLASS <<std::endl;
    std::cout<<"TTL: "<< unsigned(this->answer.TTL) <<std::endl;
    std::cout<<"RDLENGTH: "<< this->answer.RDLENGTH <<std::endl;
    std::cout<<"RDATA: ";
    for(std::uint8_t b : this->answer.RDATA)
    {
        if(b >= 97 && b <= 122)
        {
            std::cout<<b<<" ";
        }
        else std::cout<<unsigned(b)<<" ";
    }
    std::cout << std::endl;

    std::cout<<"\n\nDisplaying overall DNS message/response details: "<<std::endl;
    std::cout<<"Buffer size: "<<bytes_received<<std::endl;
    std::cout<<"Header size: "<<sizeof(this->header)<<std::endl;
    std::cout<<"Question QNAME size: "<<this->question.QNAME.size()<<std::endl;
    std::cout<<"Question Section size: "<<this->question.QNAME.size() + 4<<std::endl;
    std::cout<<"Answer NAME size: "<<this->answer.NAME.size() << std::endl;
    std::cout<<"Answer RDATA size: "<<this->answer.RDATA.size() << std::endl;
    std::cout<<"Answer Section size: "<<this->answer.NAME.size() + 10 + this->answer.RDATA.size() << std::endl;
}
