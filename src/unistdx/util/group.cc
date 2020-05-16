/*
UNISTDX — C++ library for Linux system calls.
© 2017, 2020 Ivan Gankevich

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

#include <unistdx/util/group>

#include <algorithm>
#include <ostream>

#include <unistdx/bits/entity>
#include <unistdx/config>
#include <unistdx/it/intersperse_iterator>

std::ostream&
sys::operator<<(std::ostream& out, const group& rhs) {
    out << rhs.name() << ':'
        << rhs.password() << ':'
        << rhs.id() << ':';
    std::copy(
        rhs.begin(),
        rhs.end(),
        intersperse_iterator<const char*,char>(out, ',')
    );
    return out;
}


bool
sys::find_group(gid_type gid, group& u) {
    #if defined(UNISTDX_HAVE_GETGRGID_R)
    return find_entity_r<gid_type>(gid, u, u._buf);
    #else
    return find_entity_nr<gid_type>(gid, u, u._buf);
    #endif
}

bool
sys::find_group(const char* name, group& u) {
    #if defined(UNISTDX_HAVE_GETGRNAM_R)
    return find_entity_r<gid_type>(name, u, u._buf);
    #else
    return find_entity_nr<gid_type>(name, u, u._buf);
    #endif
}
