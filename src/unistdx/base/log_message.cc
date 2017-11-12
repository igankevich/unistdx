#include "log_message"

sys::log_message::~log_message() {
	char ch = '\n';
	this->_str.buf().sputn(&ch, 1);
	this->_str.buf().pubflush();
}

