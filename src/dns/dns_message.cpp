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