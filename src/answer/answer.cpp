#include "answer.hpp"

DNS_Answer::DNS_Answer(){}

void DNS_Answer::to_network_order()
{
    TYPE = htons(TYPE);
    CLASS = htons(CLASS);
    TTL = htonl(TTL);
    RDLENGTH = htons(RDLENGTH);
}

void DNS_Answer::from_network_order()
{
    TYPE = ntohs(TYPE);        // Convert 16-bit from network to host byte order
    CLASS = ntohs(CLASS);      // Convert 16-bit from network to host byte order
    TTL = ntohl(TTL);          // Convert 32-bit from network to host byte order
    RDLENGTH = ntohs(RDLENGTH);// Convert 16-bit from network to host byte order
}