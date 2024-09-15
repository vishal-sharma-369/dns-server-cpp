#include <unistd.h>
#include "dns_message.hpp"

DNS_Message::DNS_Message(){}
void DNS_Message::to_network_order()
{
    header.to_network_order();
    for(DNS_Question question : questions)
        question.to_network_order();
    for(DNS_Answer answer : answers)
        answer.to_network_order();
}

void DNS_Message::from_network_order()
{
    header.from_network_order();
    for(DNS_Question question : questions)
        question.from_network_order();
    for(DNS_Answer answer : answers)
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
    this->questions.resize(1);
    this->questions[0].write_dns_question_section();

    // Step3: Create the DNS_Answer Section
    this->answers.resize(1);
    this->answers[0].write_dns_answer_section();

    this->to_network_order();
}

void DNS_Message::write_dns_message_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend)
{
        // Copying the response header to the response buffer
        this->header.write_dns_header_to_byte_buffer(responseBuffer, bytesToSend);

        // Copying the response question section to the response buffer
        std::uint8_t questionSectionStartIndex = sizeof(this->header);
        for(DNS_Question question : this->questions)
        {
            question.write_dns_question_section_to_byte_buffer(responseBuffer, bytesToSend, questionSectionStartIndex);
            questionSectionStartIndex += question.QNAME.size() + 4;
        }

        // Copying the response answer section to the response buffer
        // std::uint16_t answerSectionStartIndex = sizeof(this->header);
        // for(DNS_Question question : this->questions)
        // {
        //     answerSectionStartIndex += question.QNAME.size() + 4;
        // }
        std::uint16_t answerSectionStartIndex = questionSectionStartIndex;
        for(DNS_Answer answer : this->answers)
        {
            answer.write_dns_answer_section_to_byte_buffer(responseBuffer, bytesToSend, answerSectionStartIndex);
            answerSectionStartIndex += answer.NAME.size() + 10 + answer.RDATA.size();
        }

        // We reverse fields from network order back to system order, so that the original values can be used by logger for writing logs
        this->from_network_order();
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
    this->questions.resize(this->header.QDCOUNT);
    std::uint8_t questionSectionStartIndex = sizeof(this->header);
    for(int i = 0; i < this->header.QDCOUNT; i++)
    {
        std::uint16_t qname_length = compute_name_length(response_msg, bytes_received, questionSectionStartIndex);
        this->questions[i].parse_dns_question_section(response_msg, bytes_received, questionSectionStartIndex, qname_length);
        questionSectionStartIndex += this->questions[i].QNAME.size() + 4;
    }

    std::cout<<"\n\nDisplaying Question Section Details Received: "<<std::endl;
    for(int i = 0; i < this->header.QDCOUNT; i++)
    {
        std::cout<<"\nQuestion "<< i+1 <<": "<<std::endl;
        std::cout<<"QNAME: ";
        for(std::uint8_t b : this->questions[i].QNAME)
        {
            if(b >= 97 && b <= 122)
            {
                std::cout<<b<<" ";
            }
            else std::cout<<unsigned(b)<<" ";
        }
        std::cout<<std::endl;
        std::cout<<"TYPE: "<< this->questions[i].TYPE <<std::endl;
        std::cout<<"CLASS: "<< this->questions[i].CLASS <<std::endl;
    }

    // Parse dns answer section
    // std::uint16_t answerSectionStartIndex = sizeof(this->header);
    // for(int i = 0; i < this->header.QDCOUNT; i++)
    // {
    //     answerSectionStartIndex += this->questions[i].QNAME.size() + 4;
    // }

    std::uint16_t answerSectionStartIndex = questionSectionStartIndex;
    this->answers.resize(this->header.ANCOUNT);
    for(int i = 0; i < this->header.ANCOUNT; i++)
    {
        std::uint16_t name_length = compute_name_length(response_msg, bytes_received, answerSectionStartIndex);
        this->answers[i].parse_dns_answer_section(response_msg, bytes_received, answerSectionStartIndex, name_length);
        answerSectionStartIndex += this->answers[i].NAME.size() + 10 + this->answers[i].RDATA.size();
    }

    std::cout<<"\n\nDisplaying Answer Section Details Received: "<<std::endl;
    for(int i = 0; i < this->header.ANCOUNT; i++)
    {
        std::cout<<"\nAnswer " << i+1 << ": " << std::endl;
        std::cout<<"NAME: ";
        for(std::uint8_t b : this->answers[i].NAME)
        {
            if(b >= 97 && b <= 122)
            {
                std::cout<<b<<" ";
            }
            else std::cout<<unsigned(b)<<" ";
        }
        std::cout<<std::endl;
        std::cout<<"TYPE: "<< this->answers[i].TYPE <<std::endl;
        std::cout<<"CLASS: "<< this->answers[i].CLASS <<std::endl;
        std::cout<<"TTL: "<< unsigned(this->answers[i].TTL) <<std::endl;
        std::cout<<"RDLENGTH: "<< this->answers[i].RDLENGTH <<std::endl;
        std::cout<<"RDATA: ";
        for(std::uint8_t b : this->answers[i].RDATA)
        {
            if(b >= 97 && b <= 122)
            {
                std::cout<<b<<" ";
            }
            else std::cout<<unsigned(b)<<" ";
        }
        std::cout << std::endl;
    }

    std::cout<<"\n\nDisplaying overall DNS message/response details: "<<std::endl;
    std::cout<<"Buffer size: "<<bytes_received<<std::endl;
    std::cout<<"Header size: "<<sizeof(this->header)<<std::endl;
    std::cout<<"Question Count: "<<this->header.QDCOUNT<<std::endl;
    std::cout<<"Question Section size: "<< questionSectionStartIndex - 12<<std::endl;
    std::cout<<"Answer Count: "<<this->header.ANCOUNT << std::endl;
    std::cout<<"Answer Section size: "<< answerSectionStartIndex - questionSectionStartIndex << std::endl;
}
