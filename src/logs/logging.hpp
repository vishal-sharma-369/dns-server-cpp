#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <cstdint>

void write_message_to_server_log(std::uint8_t responseBuffer[], std::pair<std::uint16_t, std::uint16_t> bytes_sent, DNS_Message& response);
void write_message_to_client_log(std::uint8_t buffer[], int bytes, std::string message);

#endif