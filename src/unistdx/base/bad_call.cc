#include "bad_call"

#include <ostream>

std::ostream&
sys::operator<<(std::ostream& out, const bad_call& rhs) {
	return out
		<< rhs._file << ':'
		<< rhs._line << ':'
		<< rhs._function << ' '
		<< rhs.what();
}

