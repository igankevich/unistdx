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

#include <ostream>
#include <sstream>

#include <unistdx/ipc/process>
#include <unistdx/system/error>

#include <unistdx/config>
#if defined(UNISTDX_HAVE_BACKTRACE)
#include <execinfo.h>
#endif
#if defined(UNISTDX_HAVE_CXXABI_H)
#include <cstdlib>
#include <cstring>
#include <cxxabi.h>
#include <string>
#endif

std::vector<sys::backtrace_symbol> sys::backtrace_symbols() {
    #if defined(UNISTDX_HAVE_BACKTRACE)
    const size_t size = 4096 / sizeof(void*);
    void* buffer[size];
    const int nptrs = ::backtrace(buffer, size);
    std::vector<backtrace_symbol> symb;
    symb.reserve(nptrs);
    string buf(4096);
    if (char** symbols = ::backtrace_symbols(buffer, nptrs)) {
        for (int i=0; i<nptrs; ++i) {
            const char* name = symbols[i];
            if (!name) {
                name = "<null>";
            }
            try {
                // parse trace string
                std::string line(name);
                std::string::size_type pos0, pos1;
                pos0 = line.find_first_of('(');
                pos1 = line.find_last_of(')');
                pos1 = line.find_last_of('+', pos1);
                if (pos0 == std::string::npos || pos1 == std::string::npos) {
                    throw 1;
                }
                std::string func = line.substr(pos0+1, pos1-pos0-1);
                auto demangled_name = demangle(func.data(), buf);
                symb.emplace_back(line.substr(0, pos0), demangled_name, 0);
            } catch (...) {
                symb.emplace_back("", name, 0);
            }
        }
        std::free(symbols);
    }
    #endif
    return symb;
}

void sys::error::init(const std::string& message) {
    std::string nm(16, '\0');
    #if defined(UNISTDX_HAVE_PRCTL)
    ::prctl(PR_GET_NAME, nm.data());
    #endif
    std::stringstream tmp;
    tmp << "Exception in process \"";
    #if defined(UNISTDX_HAVE_PRCTL)
    tmp << nm.data();
    #else
    tmp << std::this_process::id();
    #endif
    tmp << "\": " << message;
    tmp << '\n';
    int i = 0;
    for (const auto& s : this->_symbols) { tmp << "    #" << i++ << ' ' << s << '\n'; }
    this->_message = tmp.str();
}

const char* sys::error::what() const noexcept {
    return this->_message.data();
}

std::ostream& sys::operator<<(std::ostream& out, const backtrace_symbol& rhs) {
    if (!rhs.name.empty()) {
        out << rhs.name << ' ';
    }
    out << '(';
    if (rhs.filename.empty()) {
        out << rhs.object << ":0x" << std::hex << rhs.address << std::dec;
    } else {
        out << rhs.filename;
        if (rhs.line != 0) {
            out << ':' << rhs.line;
        }
    }
    out << ')';
    return out;
}

const char* sys::demangle(const char* symbol, string& buf) {
    #if defined(UNISTDX_HAVE_CXXABI_H)
    try {
        size_t size = buf.capacity();
        int status = 0;
        abi::__cxa_demangle(symbol, buf.data(), &size, &status);
        if (status == 0) {
            buf.capacity(size);
            return buf.data();
        }
        return symbol;
    } catch (...) {
        return symbol;
    }
    #else
    return symbol;
    #endif
}
