#include "log_message"

namespace {

	typedef std::recursive_mutex mutex_type;
	typedef std::unique_lock<mutex_type> lock_type;

	mutex_type log_mutex;

}

void
sys::log_message::lock() noexcept {
	log_mutex.lock();
}

void
sys::log_message::unlock() noexcept {
	log_mutex.unlock();
}
