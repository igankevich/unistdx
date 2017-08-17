#include "file_stat"

std::ostream&
sys::operator<<(std::ostream& out, const file_stat& rhs) {
	return out << rhs.type() << rhs.mode() << ' '
		<< rhs.owner() << ':' << rhs.group() << ' '
		<< rhs.size();
}

