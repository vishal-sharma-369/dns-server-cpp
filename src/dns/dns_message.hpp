#ifndef DNS_MESSAGE_H
#define DNS_MESSAGE_H

#include "../header/header.hpp"
#include "../question/question.hpp"
#include "../answer/answer.hpp"

class DNS_Message
{
    public:
        DNS_Header header;
        DNS_Question question;
        DNS_Answer answer;
    DNS_Message();
    void to_network_order();
    void from_network_order();

//  DNS Response Writer Methods
    void write_dns_message();
    void write_dns_message_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend);

//  DNS Message Parser Methods
    void parse_dns_message(std::uint8_t response_msg[], ssize_t bytes_received);
};

#endif
