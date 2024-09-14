#ifndef QUESTION_H
#define QUESTION_H
#include <iostream>
#include <cstdint>
#include <vector>

class DNS_Question
{
    public:
        std::vector<char> QNAME;
        std::uint16_t TYPE;
        std::uint16_t CLASS;
    
    DNS_Question();
    void to_network_order();
};

#endif