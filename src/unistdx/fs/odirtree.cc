#include <unistdx/fs/odirtree>

sys::path
sys::copy_recursively::make_dirs(const sys::path& rhs) const {
    // LCOV_EXCL_START
    assert(!this->_src.empty());
    assert(rhs.compare(0, this->_src.size(), this->_src) == 0);
    // LCOV_EXCL_STOP
    return path(rhs.substr(this->_src.size() + 1));
}
