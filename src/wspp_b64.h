#ifndef WSPP_HEAD_B64
#define WSPP_HEAD_B64

#include <iostream>
#include <string>

const std::string b64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string hexToBase64(const std::string& hexStr)
{
    std::string base64Str;
    int val = 0;
    int bits = 0;

    for (char c : hexStr) {
        if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else return "";

        bits += 4;

        while (bits >= 6) {
            bits -= 6;
            base64Str.push_back(b64_chars[(val >> bits) & 0x3F]);
        }
    }

    if (bits > 0) {
        val <<= 6 - bits;
        base64Str.push_back(b64_chars[val & 0x3F]);
        while (bits < 6) base64Str.push_back('='), bits += 2;
    }

    return base64Str;
}

std::string stringToBase64(const std::string& input)
{
    std::string result;
    size_t inputLength = input.length();

    for (size_t i = 0; i < inputLength; i += 3) {
        uint32_t octetA = i < inputLength ? static_cast<uint8_t>(input[i]) : 0;
        uint32_t octetB = i + 1 < inputLength ? static_cast<uint8_t>(input[i + 1]) : 0;
        uint32_t octetC = i + 2 < inputLength ? static_cast<uint8_t>(input[i + 2]) : 0;

        uint32_t triple = (octetA << 16) + (octetB << 8) + octetC;

        result += b64_chars[(triple >> 18) & 0x3F];
        result += b64_chars[(triple >> 12) & 0x3F];
        result += b64_chars[(triple >> 6) & 0x3F];
        result += b64_chars[triple & 0x3F];
    }

    // Handle padding
    size_t padding = 3 - (inputLength % 3);
    if (padding != 3) {
        for (size_t i = 0; i < padding; i++) {
            result.pop_back(); // Remove extra characters
            result += '=';
        }
    }

    return result;
}

#endif
