#include "sysstream"

#include <unistdx/io/fildesbuf>

namespace {

	typedef sys::basic_fildesbuf<char,std::char_traits<char>,sys::fd_type>
	    fildesbuf_type;

}

sys::osysstream::osysstream(std::streambuf* buf):
ostream_type(nullptr) {
	this->init(buf);
}
