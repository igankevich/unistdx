#include "groupstream"
#include <mutex>

namespace {

	std::mutex group_mutex;

	inline sys::group*
	next_entry() {
		return static_cast<sys::group*>(::getgrent());
	}

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
		group* g = next_entry();
		if (g == nullptr) {
			_end = true;
		} else {
			rhs = *g;
		}
	}
	return *this;
}
