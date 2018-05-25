#include "odirtree"

sys::path
sys::copy_recursively::make_dirs(const sys::path& rhs) const {
	sys::path p;
	if (!this->_src.empty() &&
		rhs.compare(0, this->_src.size(), this->_src) == 0) {
		p = rhs.substr(this->_src.size() + 1);
	} else {
		p = rhs;
	}
	return p;
}
