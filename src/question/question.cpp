#include <arpa/inet.h>
#include "question.hpp"

DNS_Question::DNS_Question(){}
void DNS_Question::to_network_order()
{
    TYPE = htons(TYPE);
    CLASS = htons(CLASS);
}

void DNS_Question::from_network_order()
{
    TYPE = ntohs(TYPE);
    CLASS = ntohs(CLASS);
}

//  DNS Question Writer Methods
void DNS_Question::write_dns_question_section(std::vector<std::string> domains, std::uint16_t type, std::uint16_t clss)
{
    for(int i = 0; i < domains.size(); i++)
    {
        this->QNAME.push_back((uint8_t) domains[i].size());
        for(char c : domains[i])
        {
            this->QNAME.push_back(c);
        }
    }
    this->QNAME.push_back(0);

    this->CLASS = type;
    this->TYPE = clss;
}

std::string DNS_Question::get_substring(std::vector<std::uint8_t> str, std::uint16_t start, std::uint16_t end)
{
    std::string ans = "";
    for(int i = start; i <= end; i++)
    {
        ans += str[i];
    }
    return ans;
}

std::uint16_t DNS_Question::write_dns_question_section_to_byte_buffer(std::uint8_t responseBuffer[], std::uint16_t bytesToSend, std::uint8_t questionSectionStartIndex, std::unordered_map<std::string, std::uint16_t> &domain_name_to_buffer_index_pointer)
{
    int index = 0;
    while(index < this->QNAME.size())
    {
        int domain_length = this->QNAME[index];
        std::string domain = get_substring(this->QNAME, index + 1, index + domain_length);
        if(domain_name_to_buffer_index_pointer.find(domain) == domain_name_to_buffer_index_pointer.end())
        {
            if(domain != "") domain_name_to_buffer_index_pointer[domain] = questionSectionStartIndex;
            responseBuffer[questionSectionStartIndex++] = domain_length;
            std::copy(domain.begin(), domain.end(), responseBuffer + questionSectionStartIndex);
            questionSectionStartIndex += domain.size();
            index += domain_length + 1;
        }
        else 
        {
            std::uint16_t pointerIndex = domain_name_to_buffer_index_pointer[domain];
            responseBuffer[questionSectionStartIndex++] = (0xC0) | ((pointerIndex >> 8) & 0xFF);
            responseBuffer[questionSectionStartIndex++] = (pointerIndex) & 0xFF;
            // We are doing this index += 2 as index can also give us the size of qname in response buffer
            // otherwise there is no logical meaning of doing index += 2 as it is not used any further after this step
            index += 2;
            break;
        }
    }
    // The extra 4 size is used for adding the TYPE and CLASS fields of question section(as they are of 2 bytes and character is of 1 byte each, so we need a total of 4 to create response)
    responseBuffer[questionSectionStartIndex] = (this->TYPE >> 8) & 0xFF;
    responseBuffer[questionSectionStartIndex + 1] = this->TYPE & 0xFF;
    responseBuffer[questionSectionStartIndex + 2] = (this->CLASS >> 8) & 0xFF;
    responseBuffer[questionSectionStartIndex + 3] = (this->CLASS) & 0xFF;
    return index;
}

//  DNS Question Parser Methods

/*
* This function tells whether the label in question QNAME or answer NAME is a pointer or not
*/
bool DNS_Question::isPointer(std::uint8_t x)
{
    return (x & (3 << 6)) == (3 << 6);
}

void DNS_Question::parse_dns_question_section(std::uint8_t response_msg[], ssize_t bytes_received, std::uint8_t questionSectionStartIndex, std::uint16_t qname_length)
{
    // this->QNAME.resize(qname_length);
    // std::copy(response_msg + questionSectionStartIndex, response_msg + questionSectionStartIndex + qname_length, this->QNAME.begin());
    std::uint8_t originalQuestionSectionStartingIndex = questionSectionStartIndex;
    std::uint8_t length = response_msg[questionSectionStartIndex];
    std::uint16_t qname_size = 1;
    bool is_compressed = false;
    while(length != 0)
    {
        if(this->isPointer(length))
        {
            uint16_t offset = ((length & 0x3F) << 8) | static_cast<uint8_t>((response_msg[questionSectionStartIndex + 1]));
            questionSectionStartIndex = offset;
            qname_size += 2;
            is_compressed = true;
        }
        else 
        {
            for(int i = questionSectionStartIndex; i < questionSectionStartIndex + length + 1; i++)
            {
                this->QNAME.push_back(response_msg[i]);
            }
            questionSectionStartIndex += length + 1;
            if(!is_compressed) qname_size += length + 1;
        }

        length = response_msg[questionSectionStartIndex];
    }

    if(is_compressed) qname_size -= 1;
    this->QNAME.push_back(0);
    // The extra 4 size is used for adding the TYPE and CLASS fields of question section(as they are of 2 bytes and character is of 1 byte each, so we need a total of 4 to create response)
    std::uint16_t questionSectionStartIndexAfterQNAME = originalQuestionSectionStartingIndex + qname_size;
    this->TYPE = std::uint16_t(response_msg[questionSectionStartIndexAfterQNAME]) | std::uint16_t(response_msg[questionSectionStartIndexAfterQNAME + 1]) << 8;
    this->CLASS = std::uint16_t(response_msg[questionSectionStartIndexAfterQNAME + 2]) | std::uint16_t(response_msg[questionSectionStartIndexAfterQNAME + 3]) << 8;
    this->from_network_order();
}
