/*
UNISTDX — C++ library for Linux system calls.
© 2020 Ivan Gankevich

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

#include <algorithm>
#include <cstring>

#include <unistdx/base/sha2>
#include <unistdx/bits/macros>
#include <unistdx/config>
#include <unistdx/net/byte_order>

namespace {

constexpr const sys::u32 k[64] = {
0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

    // circular left shift
    UNISTDX_CONST sys::u32
    cls(int n, sys::u32 x) noexcept {
        return (x << n) | (x >> (32-n));
    }

    // circular right shift
    UNISTDX_CONST sys::u32
    rotr(sys::u32 x, int n) noexcept {
        return (x >> n) | (x << (32-n));
    }

    // right shift
    UNISTDX_CONST sys::u32
    shr(sys::u32 x, int n) noexcept {
        return x >> n;
    }

    UNISTDX_CONST sys::u32
    sigma_0(sys::u32 x) noexcept {
        return rotr(x,7) ^ rotr(x,18) ^ shr(x,3);
    }

    UNISTDX_CONST sys::u32
    sigma_1(sys::u32 x) noexcept {
        return rotr(x,17) ^ rotr(x,19) ^ shr(x,10);
    }

    UNISTDX_CONST sys::u32
    sum_0(sys::u32 x) noexcept {
        return rotr(x,2) ^ rotr(x,13) ^ rotr(x,22);
    }

    UNISTDX_CONST sys::u32
    sum_1(sys::u32 x) noexcept {
        return rotr(x,6) ^ rotr(x,11) ^ rotr(x,25);
    }

    UNISTDX_CONST sys::u32
    ch(sys::u32 x, sys::u32 y, sys::u32 z) noexcept {
        return (x & y) ^ (~x & z);
    }

    UNISTDX_CONST sys::u32
    maj(sys::u32 x, sys::u32 y, sys::u32 z) noexcept {
        return (x & y) ^ (x & z) ^ (y & z);
    }

}

void sys::sha2::insert(const char* data, std::size_t n) {
    if (n >= std::numeric_limits<size_t>::max()/8 ||
        n >= std::numeric_limits<size_t>::max()/8 - this->_length/8) {
        throw std::length_error("sha2 input is too large");
    }
    auto first = this->_bytesptr, last = this->_bytes + 64;
    auto data_end = data + n;
    while (data != data_end) {
        const auto m = std::min(last-first, data_end-data);
        first = std::copy_n(data, m, first);
        data += m;
        if (first == last) {
            process_message();
            first = this->_bytes;
        }
    }
    this->_bytesptr = first;
    this->_length += n*8;
}

void sys::sha2::clear() noexcept {
    *this = sha2();
}

void sys::sha2::finish() noexcept {
    pad_message();
}

void sys::sha2::process_message() noexcept {
    auto* w = this->_words;
    #if !defined(UNISTDX_BIG_ENDIAN)
    for (int i=0; i<16; ++i) {
        w[i] = byte_swap(w[i]);
    }
    #endif
    for (int i=16; i<64; ++i) {
        w[i] = sigma_1(w[i-2]) + w[i-7] + sigma_0(w[i-15]) + w[i-16];
    }
    u32 a = this->_digest[0];
    u32 b = this->_digest[1];
    u32 c = this->_digest[2];
    u32 d = this->_digest[3];
    u32 e = this->_digest[4];
    u32 f = this->_digest[5];
    u32 g = this->_digest[6];
    u32 h = this->_digest[7];
    for (int i=0; i<64; ++i) {
        u32 t1 = h + sum_1(e) + ch(e,f,g) + k[i] + w[i];
        u32 t2 = sum_0(a) + maj(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }
    this->_digest[0] += a,
    this->_digest[1] += b,
    this->_digest[2] += c,
    this->_digest[3] += d,
    this->_digest[4] += e,
    this->_digest[5] += f,
    this->_digest[6] += g,
    this->_digest[7] += h;
}

void sys::sha2::pad_message() noexcept {
    const u64 orig_length = this->_length;
    const int bytes_needed = sizeof(unsigned char) + sizeof(u64);
    const int bytes_avail = this->_bytes + 64 - this->_bytesptr;
    *this->_bytesptr++ = 0x80;
    if (bytes_avail < bytes_needed) {
        // there is not enough space for a 64-bit message size
        std::fill(this->_bytesptr, this->_bytes + 64, '\0');
        this->process_message();
        this->_bytesptr = this->_bytes;
    }
    // pad the block
    std::fill(this->_bytesptr, this->_bytes + 64 - sizeof(u64), '\0');
    // store the size of the original message
    // in the last double word
    this->_dwords[7] = to_network_format(orig_length);
    this->process_message();
}

std::string sys::sha2::to_string() const {
    std::string s;
    s.reserve(64);
    for (int i=0; i<8; ++i) {
        for (int j=1; j<=8; ++j) {
            s += "0123456789abcdef"[(this->_digest[i] >> (sizeof(u32)*8 - 4*j)) & 0xf];
        }
    }
    return s;
}
