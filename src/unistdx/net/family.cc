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

#include <unistdx/net/family>

#include <limits>

#include <unistdx/base/types>

namespace {

    typedef sys::u8 raw_family_type;

    enum struct portable_family_type: raw_family_type {
        unspecified = 0,
        inet = 1,
        inet6 = 2,
        unix = 3
        #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
        , netlink = 4
        #endif
    };

    inline portable_family_type
    map_family_type(sys::family_type t) {
        switch (t) {
            case sys::family_type::unspecified: return portable_family_type::unspecified;
            case sys::family_type::inet: return portable_family_type::inet;
            case sys::family_type::inet6: return portable_family_type::inet6;
            case sys::family_type::unix: return portable_family_type::unix;
            #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
            case sys::family_type::netlink: return portable_family_type::netlink;
            #endif
            default: return portable_family_type(
                std::numeric_limits<sys::sa_family_type>::max()
            );
        }
    };

    inline sys::family_type
    map_family_type(portable_family_type t) {
        switch (t) {
            case portable_family_type::unspecified: return sys::family_type::unspecified;
            case portable_family_type::inet: return sys::family_type::inet;
            case portable_family_type::inet6: return sys::family_type::inet6;
            case portable_family_type::unix: return sys::family_type::unix;
            #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
            case portable_family_type::netlink: return sys::family_type::netlink;
            #endif
            default: return sys::family_type(
                std::numeric_limits<sys::sa_family_type>::max()
            );
        }
    };

}

sys::bstream&
sys::operator<<(bstream& out, family_type rhs) {
    return out << static_cast<raw_family_type>(map_family_type(rhs));
}

sys::bstream&
sys::operator>>(bstream& in, family_type& rhs) {
    raw_family_type raw;
    in >> raw;
    rhs = map_family_type(static_cast<portable_family_type>(raw));
    return in;
}

const char*
sys::to_string(family_type rhs) {
    switch (rhs) {
    case family_type::unspecified: return "unspecified";
    case family_type::inet: return "inet";
    case family_type::inet6: return "inet6";
    case family_type::unix: return "unix";
    #if defined(UNISTDX_HAVE_LINUX_NETLINK_H)
    case family_type::netlink: return "netlink";
    #endif
    default: return "unknown";
    }
}

std::ostream&
sys::operator<<(std::ostream& out, const family_type& rhs) {
    return out << to_string(rhs);
}
