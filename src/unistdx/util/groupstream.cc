#include "groupstream"

#include <mutex>

namespace {

	std::mutex group_mutex;

}

sys::groupstream::groupstream():
_end(false) {
	group_mutex.lock();
	::setgrent();
}

sys::groupstream::~groupstream() {
	::endgrent();
	group_mutex.unlock();
}

sys::groupstream&
sys::groupstream::operator>>(group& rhs) {
	if (*this) {
		group_type* g = ::getgrent();
		if (g == nullptr) {
			this->_end = true;
		} else {
			rhs = *g;
		}
	}
	return *this;
}
