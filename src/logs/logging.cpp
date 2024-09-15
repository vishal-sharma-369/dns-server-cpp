#include <iostream>
#include "../dns/dns_message.hpp"
#include "logging.hpp"

std::uint16_t compute_question_section_size(DNS_Message response, int QDCOUNT)
{
    std::uint16_t size = 0;
    for(int i = 0; i < QDCOUNT; i++)
    {
        size += response.questions[i].QNAME.size() + 4;
    }
    return size;
}

std::uint16_t compute_answer_section_size(DNS_Message response, int ANCOUNT)
{
    std::uint16_t size = 0;
    for(int i = 0; i < ANCOUNT; i++)
    {
        size += response.answers[i].NAME.size() + 10 + response.answers[i].RDATA.size();
    }
    return size;
}

void write_message_to_server_log(std::uint8_t responseBuffer[], int bytes_sent, DNS_Message& response)
{
    std::cout<<std::endl<<"DNS Message Sent: ";
    for(int i = 0; i < bytes_sent; i++)
    {
        // std::cout<<std::uint8_t(buffer[i]);
        if(responseBuffer[i] >= 97 && responseBuffer[i] <= 122)
        {
            std::cout<<responseBuffer[i]<<" ";
        }
        else std::cout<<unsigned(responseBuffer[i])<<" ";
    }
    std::cout<<std::endl;

    std::cout<<std::endl<<"DNS Message Sent in Hexadecimal form: ";
    for(int i = 0; i < bytes_sent; i++)
    {
        // std::cout<<std::uint8_t(buffer[i]);
        if(responseBuffer[i] >= 97 && responseBuffer[i] <= 122)
        {
            std::cout<<std::hex<<unsigned(responseBuffer[i]);
        }
        else std::cout<<std::hex<<unsigned(responseBuffer[i]);
        std::cout<<" ";
    }
    std::cout<<"\n\n";

    // Reset output stream to decimal outputs
    std::cout<<std::dec;

    std::cout<<"\n\nDisplaying overall DNS message/response details: "<<std::endl;
    std::cout<<"Buffer size: "<<bytes_sent<<std::endl;
    std::cout<<"Header size: "<<sizeof(response.header)<<std::endl;
    std::cout<<"Question Count: "<<response.header.QDCOUNT<<std::endl;
    std::cout<<"Question Section size: "<< compute_question_section_size(response, response.header.QDCOUNT) <<std::endl;
    std::cout<<"Answer Count: "<<response.header.ANCOUNT << std::endl;
    std::cout<<"Answer Section size: "<< compute_answer_section_size(response, response.header.ANCOUNT) << std::endl;
}

void write_message_to_client_log(std::uint8_t responseBuffer[], int bytes_received)
{
    std::cout<<std::endl<<"DNS Response received: ";
    for(int i = 0; i < bytes_received; i++)
    {
        // std::cout<<std::uint8_t(buffer[i]);
        if(responseBuffer[i] >= 97 && responseBuffer[i] <= 122)
        {
            std::cout<<responseBuffer[i]<<" ";
        }
        else std::cout<<unsigned(responseBuffer[i])<<" ";
    }
    std::cout<<std::endl;

    std::cout<<std::endl<<"DNS Response received in Hexadecimal form: ";
    for(int i = 0; i < bytes_received; i++)
    {
        // std::cout<<std::uint8_t(buffer[i]);
        if(responseBuffer[i] >= 97 && responseBuffer[i] <= 122)
        {
            std::cout<<std::hex<<unsigned(responseBuffer[i]);
        }
        else std::cout<<std::hex<<unsigned(responseBuffer[i]);
        std::cout<<" ";
    }
    std::cout<<"\n\n";

    // Reset output stream to decimal outputs
    std::cout<<std::dec;
}