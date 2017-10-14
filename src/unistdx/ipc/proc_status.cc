#include "proc_status"

#include <unistdx/base/make_object>

std::ostream&
sys::operator<<(std::ostream& out, const proc_status& rhs) {
	if (rhs.exited()) {
		out << make_object(
			"status",
			"exited",
			"exit_code",
			rhs.exit_code()
		               );
	} else if (rhs.killed()) {
		out << make_object(
			"status",
			"killed",
			"term_signal",
			rhs.term_signal()
		                 );
	} else if (rhs.stopped()) {
		out << make_object(
			"status",
			"stopped",
			"stop_signal",
			rhs.stop_signal()
		                 );
	} else if (rhs.continued()) {
		out << make_object("status", "continued");
	} else {
		out << make_object("status", "unknown");
	}
	return out;
}
