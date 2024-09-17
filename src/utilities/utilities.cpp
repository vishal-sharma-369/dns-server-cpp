#include "utilities.hpp"
#include <sstream>

std::vector<std::string> get_domains_from_name(std::string qname)
{
    std::vector<std::string> domains;
    std::stringstream ss(qname);
    std::string domain;
    while(!ss.eof())
    {
        getline(ss, domain, '.');
        domains.push_back(domain);
    }
    return domains;
}

/*
* This function tells whether the label in question QNAME or answer NAME is a pointer or not
*/
bool isPointer(std::uint8_t x)
{
    return (x & (3 << 6)) == (3 << 6);
}

std::string get_substring(std::vector<std::uint8_t> str, std::uint16_t start, std::uint16_t end)
{
    std::string ans = "";
    for(int i = start; i <= end; i++)
    {
        ans += str[i];
    }
    return ans;
}
