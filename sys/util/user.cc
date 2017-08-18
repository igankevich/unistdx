#include "user"

std::ostream&
sys::operator<<(std::ostream& out, const user& rhs) {
	return out
		   << rhs.name() << ':'
		   << rhs.password() << ':'
		   << rhs.id() << ':'
		   << rhs.group_id() << ':'
		   << (rhs.real_name() ? rhs.real_name() : "") << ':'
		   << rhs.home() << ':'
		   << rhs.shell();
}

