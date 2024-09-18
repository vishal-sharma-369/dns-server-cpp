#include <iostream>
#include "../dns/dns_message.hpp"
#include "logging.hpp"

void write_message_to_server_log(std::uint8_t responseBuffer[], std::pair<std::uint16_t, std::uint16_t> responseSizeInResponseBuffer, DNS_Message& response)
{
    std::cout<<std::endl<<"DNS Message Sent: ";
    for(int i = 0; i < responseSizeInResponseBuffer.second ; i++)
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
    for(int i = 0; i < responseSizeInResponseBuffer.second ; i++)
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
    std::cout<<"Buffer size: "<< responseSizeInResponseBuffer.second <<std::endl;
    std::cout<<"Header size: "<<sizeof(response.header)<<std::endl;
    std::cout<<"Question Count: "<<response.header.QDCOUNT<<std::endl;
    std::cout<<"Question Section size: "<< responseSizeInResponseBuffer.first - 12 <<std::endl;
    std::cout<<"Answer Count: "<<response.header.ANCOUNT << std::endl;
    std::cout<<"Answer Section size: "<< responseSizeInResponseBuffer.second - responseSizeInResponseBuffer.first << std::endl;
}

void write_dns_message_buffer_to_console(std::uint8_t buffer[], int bytes, std::string message)
{
    std::cout<<std::endl<<message << ": ";
    for(int i = 0; i < bytes; i++)
    {
        // std::cout<<std::uint8_t(buffer[i]);
        if(buffer[i] >= 97 && buffer[i] <= 122)
        {
            std::cout<<buffer[i]<<" ";
        }
        else std::cout<<unsigned(buffer[i])<<" ";
    }
    std::cout<<std::endl;

    std::cout<<std::endl<<message<<" in Hexadecimal form: ";
    for(int i = 0; i < bytes; i++)
    {
        // std::cout<<std::uint8_t(buffer[i]);
        if(buffer[i] >= 97 && buffer[i] <= 122)
        {
            std::cout<<std::hex<<unsigned(buffer[i]);
        }
        else std::cout<<std::hex<<unsigned(buffer[i]);
        std::cout<<" ";
    }
    std::cout<<"\n\n";

    // Reset output stream to decimal outputs
    std::cout<<std::dec;
}
