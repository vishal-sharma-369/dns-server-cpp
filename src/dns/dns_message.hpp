#ifndef DNS_MESSAGE_H
#define DNS_MESSAGE_H

#include <unistd.h>
#include "../header/header.hpp"
#include "../question/question.hpp"
#include "../answer/answer.hpp"

class DNS_Message
{
    public:
        DNS_Header header;
        std::vector<DNS_Question> questions;
        std::vector<DNS_Answer> answers;
    DNS_Message();
    void to_network_order();
    void from_network_order();

//  DNS Response Writer Methods
void write_dns_message(std::uint16_t ID, std::uint16_t FLAGS, std::uint16_t QDCOUNT, 
std::uint16_t ANCOUNT, std::uint16_t NSCOUNT, std::uint16_t ARCOUNT, std::vector<std::vector<std::string>> questions, std::vector<std::vector<std::string>> answers);
    std::pair<std::uint16_t, std::uint16_t> write_dns_message_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend);
    void create_dns_query();

// DNS Response Parsing Methods
    std::uint16_t compute_name_length(std::uint8_t response_msg[], ssize_t bytes_received, std::uint8_t start_index);
    void parse_dns_message(std::uint8_t response_msg[], ssize_t bytes_received);
};

#endif
