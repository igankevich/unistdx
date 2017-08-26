#include "proc_info"

#include <ostream>

std::ostream&
sys::operator<<(std::ostream& out, const proc_info& rhs) {
	out << "pid=" << rhs.pid() << ',';
	out << "status=" << rhs.status_string() << ',';
	if (rhs.exited()) {
		out << "exit_code=" << rhs.exit_code();
	} else {
		out << "signal=" << rhs.term_signal();
	}
	return out;
}

