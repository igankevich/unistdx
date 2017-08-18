#include "process"
#include <stdx/debug.hh>

std::ostream&
sys::operator<<(std::ostream& out, const process& rhs) {
	return out << stdx::make_object("id", rhs.id(), "gid", rhs.group_id());
}

