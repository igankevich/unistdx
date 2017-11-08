#include "log_message"

#include <unistdx/io/fildesbuf>

namespace {

	typedef sys::basic_fildesbuf<char,std::char_traits<char>,sys::fd_type>
	    fildesbuf_type;

	thread_local fildesbuf_type threadbuf(STDERR_FILENO, 0, 256);

}

thread_local sys::osysstream sys::log(&threadbuf);

sys::log_message::~log_message() {
	char ch = '\n';
	threadbuf.sputn(&ch, 1);
	threadbuf.pubflush();
}
