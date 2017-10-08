#include "byte_buffer"

#include <cassert>

#include <unistdx/base/check>
#include <unistdx/config>

#if !defined(UNISTDX_HAVE_MMAP)
#error mmap is needed to implement byte_buffer
#else
#include <sys/mman.h>
#endif

namespace {

	void
	init_pages(void* addr, size_t n) {
		UNISTDX_CHECK(::mlock(addr, n));
		UNISTDX_CHECK(::madvise(addr, n, MADV_SEQUENTIAL));
		UNISTDX_CHECK(::madvise(addr, n, MADV_DONTFORK));
		UNISTDX_CHECK(::madvise(addr, n, MADV_DONTDUMP));
	}

}

sys::byte_buffer::byte_buffer(size_type size):
_size(size) {
	if (this->_size > 0) {
		UNISTDX_CHECK2(
			this->_data =
				static_cast<value_type*>(
					::mmap(
						nullptr,
						size,
						PROT_READ | PROT_WRITE,
						MAP_PRIVATE| MAP_ANONYMOUS,
						-1,
						0
					)),
			MAP_FAILED
		);
		init_pages(this->_data, this->_size);
	}
}

sys::byte_buffer::~byte_buffer() {
	if (this->_data) {
		int ret = ::munmap(this->_data, this->_size);
		if (ret == -1) {
			std::terminate();
		}
	}
}

void
sys::byte_buffer::resize(size_type new_size) {
	assert(this->_data);
	UNISTDX_CHECK2(
		this->_data =
			static_cast<value_type*>(
				::mremap(
					this->_data,
					this->_size,
					new_size,
					MREMAP_MAYMOVE
				)),
		MAP_FAILED
	);
	this->_size = new_size;
	init_pages(this->_data, this->_size);
}
