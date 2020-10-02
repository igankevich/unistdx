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

#include <cassert>
#include <cstring>

#include <unistdx/base/byte_buffer>
#include <unistdx/base/check>
#include <unistdx/bits/mman>
#include <unistdx/util/system>

namespace {

    inline void init_pages(void* addr, size_t n) {
        // LCOV_EXCL_START
        UNISTDX_CHECK(::madvise(addr, n, MADV_DONTFORK));
        UNISTDX_CHECK(::madvise(addr, n, MADV_DONTDUMP));
        // LCOV_EXCL_STOP
    }

    inline void* do_mmap(size_t size) {
        void* ptr = ::mmap(
            nullptr,
            size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE| MAP_ANONYMOUS,
            -1,
            0
        );
        UNISTDX_CHECK2(ptr, MAP_FAILED);
        return ptr;
    }

    inline void do_unmap(void* ptr, size_t size) {
        if (ptr) { UNISTDX_CHECK(::munmap(ptr, size)); }
    }

    inline void* do_mremap(void* ptr, size_t old_size, size_t new_size) {
        ptr = ::mremap(ptr, old_size, new_size, MREMAP_MAYMOVE);
        UNISTDX_CHECK2(ptr, MAP_FAILED);
        return ptr;
    }

}

sys::byte_buffer::byte_buffer(const byte_buffer& rhs):
_size(rhs._size), _position(rhs._position), _limit(rhs._limit), _order(rhs._order) {
    if (size() > 0) {
        this->_data = static_cast<value_type*>(do_mmap(size())),
        init_pages(data(), size());
        std::memcpy(data(), rhs.data(), size());
    }
}

sys::byte_buffer& sys::byte_buffer::operator=(const byte_buffer& rhs) {
    if (this == &rhs) { return *this; }
    resize(rhs.size());
    std::memcpy(data(), rhs.data(), size());
    this->_position = rhs._position;
    this->_limit = rhs._limit;
    this->_order = rhs._order;
    return *this;
}

sys::byte_buffer::byte_buffer(size_type size):
_size(size), _limit(size) {
    if (size > 0) {
        this->_data = static_cast<value_type*>(do_mmap(size)),
        init_pages(this->_data, this->_size);
    }
}

sys::byte_buffer::~byte_buffer() noexcept {
    if (this->_data) {
        int ret = ::munmap(this->_data, this->_size);
        if (ret == -1) { std::terminate(); }
    }
}

void
sys::byte_buffer::resize(size_type new_size) {
    if (new_size == 0) {
        do_unmap(this->_data, this->_size);
        this->_data = nullptr;
        this->_size = 0;
        this->_limit = 0;
        return;
    }
    if (this->_size == 0) {
        this->_data = static_cast<value_type*>(do_mmap(new_size));
    } else {
        this->_data = static_cast<value_type*>(do_mremap(this->_data, this->_size, new_size));
    }
    this->_size = new_size;
    this->_limit = new_size;
    init_pages(this->_data, this->_size);
}

void
sys::byte_buffer::grow() {
    // LCOV_EXCL_START
    if (this->max_size() / 2 < this->_size) {
        throw std::length_error("byte_buffer size is too large");
    }
    // LCOV_EXCL_STOP
    this->resize(this->_size == 0 ? page_size() : this->_size*2);
}

auto
sys::byte_buffer::write(const_pointer src, size_type n) -> size_type {
    while (n > remaining()) { grow(); }
    std::memcpy(data()+position(), src, n);
    this->_position += n;
    return n;
}

auto
sys::byte_buffer::read(pointer dst, size_type n) -> size_type {
    if (n > remaining()) { throw std::range_error("sys::byte_buffer::read"); }
    std::memcpy(dst, data()+position(), n);
    this->_position += n;
    return n;
}

void
sys::byte_buffer::peek(pointer dst, size_type n) {
    if (n > remaining()) { throw std::range_error("sys::byte_buffer::peek"); }
    std::memcpy(dst, data()+position(), n);
}

void
sys::byte_buffer::bump(size_type n) {
    while (n > remaining()) { grow(); }
    this->_position += n;
}

void
sys::byte_buffer::compact() noexcept {
    auto n = remaining();
    std::memmove(data(), data()+position(), n);
    this->_position = n;
    this->_limit = size();
}

void
sys::byte_buffer::clear() noexcept {
    this->_position = 0;
    this->_limit = size();
    std::memset(data(), 0, size());
}
