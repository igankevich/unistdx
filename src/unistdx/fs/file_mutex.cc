#include <unistdx/fs/file_mutex>

#include <unistdx/base/check>

bool
sys::file_mutex::try_lock(file_lock_type tp) {
    int ret = ::flock(this->_fd, int(tp) | LOCK_NB);
    bool result = true;
    if (ret == -1) {
        if (errno == EWOULDBLOCK) {
            result = false;
        } else {
            UNISTDX_THROW_BAD_CALL();
        }
    }
    return result;
}
