#include "userstream"
#include <mutex>

namespace {

	std::mutex usermutex;

}

sys::userstream::userstream():
_end(false) {
	usermutex.lock();
	::setpwent();
}

sys::userstream::~userstream() {
	::endpwent();
	usermutex.unlock();
}

sys::userstream&
sys::userstream::operator>>(user& rhs) {
	if (*this) {
		passwd_type* u = ::getpwent();
		if (u == nullptr) {
			this->_end = true;
		} else {
			rhs = *u;
		}
	}
	return *this;
}

