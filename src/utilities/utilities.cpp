#include "utilities.hpp"
#include <sstream>
#include <random>

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


std::uint16_t generate_unique_dns_id() {
    // Use a random number generator to generate a 16-bit identifier
    std::random_device rd;  // Non-deterministic random number generator
    std::mt19937 gen(rd()); // Mersenne Twister engine seeded with random_device
    std::uniform_int_distribution<std::uint16_t> dis(0, 0xFFFF);  // Range for 16-bit unsigned int

    return dis(gen);  // Generate and return the random 16-bit ID
}