/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2018, 2020 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <unistdx/base/base64>

#include <unistdx/base/types>

namespace {

    const unsigned char base64_alphabet[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
        'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
        'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
        'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'
    };

    const unsigned char base64_table[128] = {
        77,77,77,77,77,77,77,77,77,77,77,77,77,77,
        77,77,77,77,77,77,77,77,77,77,77,77,77,77,
        77,77,77,77,77,77,77,77,77,77,77,77,77,77,
        77,62,77,77,77,63,52,53,54,55,56,57,58,59,
        60,61,77,77,77,77,77,77,77,0,1,2,3,4,
        5,6,7,8,9,10,11,12,13,14,15,16,17,18,
        19,20,21,22,23,24,25,77,77,77,77,77,77,26,
        27,28,29,30,31,32,33,34,35,36,37,38,39,40,
        41,42,43,44,45,46,47,48,49,50,51,77,77,77,
        77,77,
    };

    const unsigned char pad_character = '=';

    using sys::u8;
    using sys::u32;


    inline unsigned char
    char_to_index(unsigned char ch) {
        if (ch >= 128) {
            throw std::invalid_argument("bad base64 string");
        }
        unsigned char result = base64_table[ch];
        if (result == 77) {
            throw std::invalid_argument("bad base64 string");
        }
        return result;
    }

}

void
sys::base64_encode(const char* first, size_t n, char* result) noexcept {
    const size_t rem = n%3;
    const size_t m = (rem == 0) ? n : (n-rem);
    for (size_t i=0; i<m; i+=3) {
        auto b0 = *first;
        auto b1 = *++first;
        auto b2 = *++first;
        auto i3 = ((b0 >> 2) & 0x3f);
        auto i2 = ((b1 >> 4) & 0xf) | ((b0 & 0x3) << 4);
        auto i1 = ((b2 >> 6) & 0x3) | ((b1 & 0xf) << 2);
        auto i0 = b2 & 0x3f;
        *result++ = base64_alphabet[i3];
        *result++ = base64_alphabet[i2];
        *result++ = base64_alphabet[i1];
        *result++ = base64_alphabet[i0];
        ++first;
    }
    if (rem == 1) {
        auto b0 = *first;
        auto i3 = (b0 >> 2) & 0x3f;
        auto i2 = (b0 & 0x3) << 4;
        *result++ = base64_alphabet[i3];
        *result++ = base64_alphabet[i2];
        *result++ = pad_character;
        *result++ = pad_character;
    } else if (rem == 2) {
        auto b0 = *first;
        auto b1 = *++first;
        auto i3 = ((b0 >> 2) & 0x3f);
        auto i2 = ((b1 >> 4) & 0xf) | ((b0 & 0x3) << 4);
        auto i1 = (b1 & 0xf) << 2;
        *result++ = base64_alphabet[i3];
        *result++ = base64_alphabet[i2];
        *result++ = base64_alphabet[i1];
        *result++ = pad_character;
    }
}

size_t
sys::base64_decode(const char* first, size_t n, char* result) {
    const char* old = result;
    if (n%4 != 0) { throw std::invalid_argument("bad base64 string"); }
    if (n == 0) { return 0; }
    n -= 4;
    for (size_t i=0; i<n; i+=4) {
        auto i3 = char_to_index(*first);
        auto i2 = char_to_index(*++first);
        auto i1 = char_to_index(*++first);
        auto i0 = char_to_index(*++first);
        *result++ = ((i2 >> 4) & 0x3) | ((i3 & 0x3f) << 2);
        *result++ = ((i1 >> 2) & 0xf) | ((i2 & 0xf) << 4);
        *result++ = (i0 & 0x3f) | ((i1 & 0x3) << 6);
        ++first;
    }
    // process last four bytes
    if (first[2] == pad_character && first[3] == pad_character) {
        auto i3 = char_to_index(*first);
        auto i2 = char_to_index(*++first);
        *result++ = ((i2 >> 4) & 0x3) | ((i3 & 0x3f) << 2);
    } else if (first[3] == pad_character) {
        auto i3 = char_to_index(*first);
        auto i2 = char_to_index(*++first);
        auto i1 = char_to_index(*++first);
        *result++ = ((i2 >> 4) & 0x3) | ((i3 & 0x3f) << 2);
        *result++ = ((i1 >> 2) & 0xf) | ((i2 & 0xf) << 4);
    } else {
        auto i3 = char_to_index(*first);
        auto i2 = char_to_index(*++first);
        auto i1 = char_to_index(*++first);
        auto i0 = char_to_index(*++first);
        *result++ = ((i2 >> 4) & 0x3) | ((i3 & 0x3f) << 2);
        *result++ = ((i1 >> 2) & 0xf) | ((i2 & 0xf) << 4);
        *result++ = (i0 & 0x3f) | ((i1 & 0x3) << 6);
    }
    return result - old;
}
