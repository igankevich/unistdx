#include "process"
#include <unistdx/base/make_object>

std::ostream&
sys::operator<<(std::ostream& out, const process& rhs) {
	return out << make_object("id", rhs.id(), "gid", rhs.group_id());
}

