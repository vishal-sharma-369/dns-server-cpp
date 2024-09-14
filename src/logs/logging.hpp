#ifndef LOGGING_H
#define LOGGING_H

#include <cstdint>

void write_message_to_server_log(std::uint8_t responseBuffer[], int bytes_sent, DNS_Message& response);
void write_message_to_client_log(std::uint8_t responseBuffer[], int bytes_received);

#endif