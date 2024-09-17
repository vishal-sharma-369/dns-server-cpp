#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <cstdint>
#include <vector>

std::vector<std::string> get_domains_from_name(std::string qname);
bool isPointer(std::uint8_t x);
std::string get_substring(std::vector<std::uint8_t> str, std::uint16_t start, std::uint16_t end);
std::uint16_t generate_unique_dns_id();

#endif