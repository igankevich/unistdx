#include "userstream"
#include <mutex>

namespace {

	std::mutex usermutex;

	inline sys::user*
	next_entry() {
		return static_cast<sys::user*>(::getpwent());
	}

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
		user* u = next_entry();
		if (u == nullptr) {
			_end = true;
		} else {
			rhs = *u;
		}
	}
	return *this;
}

