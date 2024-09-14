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