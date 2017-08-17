#include "odirtree"
#include "mkdirs"

sys::path
sys::copy_recursively::make_dirs(const sys::path& rhs) const {
	sys::path p;
	if (!this->_src.empty() && rhs.find(this->_src) == 0) {
		p = rhs.substr(this->_src.size() + 1);
	} else {
		p = rhs;
	}
	mkdirs(this->_dest, p);
	return p;
}
