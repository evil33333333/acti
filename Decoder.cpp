#include "Decoder.h"
#include <iostream>
#include <array>
#include <vector>
#include <iomanip>
#include <windows.h>


std::string Decoder::DecodeSystemProductKey(std::vector<unsigned char> digitalProductId) {
    const int keyStartIndex = 52;
    const int keyEndIndex = keyStartIndex + 15;
    // chars allowed in a product key
    std::array<char, 24> digits = {
        'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'M', 'P', 'Q', 'R',
            'T', 'V', 'W', 'X', 'Y', '2', '3', '4', '6', '7', '8', '9',
    };
    // 29 characters for the product key inclduing the dashes
    const int decodeLength = 29;
    const int decodeStringLength = 15;
    // array to store the decoded characters
    std::array<char, decodeLength> decoded_characters{};
    std::vector<unsigned char> hex_product_id;
    for (int i = keyStartIndex; i <= keyEndIndex; i++)
    {
        hex_product_id.push_back(digitalProductId.at(i));
    }
    for (int i = decodeLength - 1; i >= 0; i--)
    {
        if ((i + 1) % 6 == 0)
        {
            decoded_characters[i] = '-';
        }
        else
        {
            int digitMapIndex = 0;
            for (int j = decodeStringLength - 1; j >= 0; j--)
            {
                unsigned char byteValue = (digitMapIndex << 8) | (unsigned char)hex_product_id[j];
                hex_product_id[j] = (unsigned char)(byteValue / 24);
                digitMapIndex = byteValue % 24;
                decoded_characters[i] = digits[digitMapIndex];
            }
        }
    }
    std::string product_key;
    for (auto const& character: decoded_characters) {
        product_key += character;
    }
    return product_key;
    
}

std::vector<unsigned char> Decoder::GetDigitalProductId() {
    char value[255];
    DWORD BufferSize = sizeof(value);
    std::vector<unsigned char> digital_product_id;

    if (RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\DefaultProductKey", "DigitalProductId", RRF_RT_ANY, NULL, value, &BufferSize) == ERROR_SUCCESS)
    {
        for (DWORD i = 0; i < BufferSize; ++i) {
            digital_product_id.push_back(value[i]);
        }
    }
    return digital_product_id;
}