#include <arpa/inet.h>
#include "header.hpp"

DNS_Header::DNS_Header(){}
void DNS_Header::to_network_order()
{
    ID = htons(ID);
    FLAGS = htons(FLAGS);
    QDCOUNT = htons(QDCOUNT);
    ANCOUNT = htons(ANCOUNT);
    NSCOUNT = htons(NSCOUNT);
    ARCOUNT = htons(ARCOUNT);
}

void DNS_Header::from_network_order()
{
    ID = ntohs(ID);
    FLAGS = ntohs(FLAGS);
    QDCOUNT = ntohs(QDCOUNT);
    ANCOUNT = ntohs(ANCOUNT);
    NSCOUNT = ntohs(NSCOUNT);
    ARCOUNT = ntohs(ARCOUNT);
}