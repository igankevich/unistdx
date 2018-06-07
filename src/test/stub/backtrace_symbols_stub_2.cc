#include <execinfo.h>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <new>
#include <vector>

char**
backtrace_symbols(void *const *, int n) {
	const size_t entry_size = 4;
	union Bytes {
		char* ptr;
		char chars[sizeof(ptr)];
	};
	// each entry occupies 4 bytes (3 characters and 1 null terminator)
	// at the beginning of the array we store the pointers to these entries
	// (align)
	char* buf = static_cast<char*>(
		std::malloc((sizeof(Bytes)*n + sizeof(char)*entry_size*n) | sizeof(Bytes))
	);
	if (!buf) {
		throw std::bad_alloc();
	}
	// skip pointers area
	char* pbuf = buf + sizeof(Bytes)*n;
	assert(size_t(pbuf) % sizeof(Bytes) == 0);
	// put all entries to the buffer and record their pointers
	std::vector<char*> ptrs(n);
	for (int i=0; i<n; ++i) {
		char entry[entry_size] = {'e', 'n', char('0'+std::min(i,9)), 0};
		std::copy_n(entry, entry_size, pbuf);
		ptrs[i] = pbuf;
		pbuf += entry_size;
	}
	pbuf = buf;
	for (int i=0; i<n; ++i) {
		Bytes bytes;
		bytes.ptr = ptrs[i];
		std::copy_n(bytes.chars, sizeof(bytes.chars), pbuf);
		pbuf += sizeof(bytes.chars);
	}
	return reinterpret_cast<char**>(buf);
}
