#ifndef DNS_MESSAGE_H
#define DNS_MESSAGE_H

#include "../header/header.hpp"
#include "../question/question.hpp"

class DNS_Message
{
    public:
        DNS_Header header;
        DNS_Question question;
    DNS_Message();
    void to_network_order();
};

#endif
