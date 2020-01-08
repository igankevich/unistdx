#include <cassert>
#include <cstring>

#include <unistdx/base/byte_buffer>
#include <unistdx/base/check>
#include <unistdx/bits/mman>

namespace {

    void
    init_pages(void* addr, size_t n) {
        // LCOV_EXCL_START
        UNISTDX_CHECK(::madvise(addr, n, MADV_DONTFORK));
        UNISTDX_CHECK(::madvise(addr, n, MADV_DONTDUMP));
        // LCOV_EXCL_STOP
    }

}

sys::byte_buffer::byte_buffer(size_type size):
_size(size), _limit(size) {
    if (this->_size > 0) {
        void* ptr = ::mmap(
            nullptr,
            size,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE| MAP_ANONYMOUS,
            -1,
            0
        );
        UNISTDX_CHECK2(ptr, MAP_FAILED);
        this->_data = static_cast<value_type*>(ptr),
        init_pages(this->_data, this->_size);
    }
}

sys::byte_buffer::~byte_buffer() {
    if (this->_data) {
        int ret = ::munmap(this->_data, this->_size);
        // LCOV_EXCL_START
        if (ret == -1) {
            std::terminate();
        }
        // LCOV_EXCL_STOP
    }
}

void
sys::byte_buffer::resize(size_type new_size) {
    assert(this->_data); // LCOV_EXCL_LINE
    void* ptr = ::mremap(this->_data, this->_size, new_size, MREMAP_MAYMOVE);
    UNISTDX_CHECK2(ptr, MAP_FAILED);
    this->_data = static_cast<value_type*>(ptr);
    this->_size = new_size;
    this->_limit = new_size;
    init_pages(this->_data, this->_size);
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
    std::memcpy(dst, data()+position(), n);
}

void
sys::byte_buffer::bump(size_type n) {
    while (n > remaining()) { grow(); }
    this->_position += n;
}

void
sys::byte_buffer::compact() {
    auto n = remaining();
    std::memmove(data(), data()+position(), n);
    this->_position = n;
    this->_limit = size();
}

void
sys::byte_buffer::clear() {
    this->_position = 0;
    this->_limit = size();
    std::memset(data(), 0, size());
}
