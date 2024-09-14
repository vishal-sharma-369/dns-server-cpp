#include <arpa/inet.h>
#include "question.hpp"

DNS_Question::DNS_Question(){}
void DNS_Question::to_network_order()
{
    TYPE = htons(TYPE);
    CLASS = htons(CLASS);
}
