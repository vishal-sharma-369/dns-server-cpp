#include <iostream>
#include "../dns/dns_message.hpp"
#include "logging.hpp"

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
    std::cout<<"Message Buffer size: "<<bytes_sent<<std::endl;
    std::cout<<"Message Header size: "<<sizeof(response.header)<<std::endl;
    std::cout<<"Message Question Section size: "<<response.question.QNAME.size() + 4<<std::endl;
    std::cout<<"Message Question QNAME size: "<<response.question.QNAME.size()<<std::endl;
    std::cout<<"Answer Section size: "<<response.answer.NAME.size() + 10 + response.answer.RDATA.size() << std::endl;
    std::cout<<"Answer NAME size: "<<response.answer.NAME.size() << std::endl;
    std::cout<<"Answer RDATA size: "<<response.answer.RDATA.size() << std::endl;
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