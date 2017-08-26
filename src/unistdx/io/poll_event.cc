#include "poll_event"
#include <unistdx/base/make_object>

std::ostream&
sys::operator<<(std::ostream& out, const poll_event& rhs) {
	const char rev[] = {
		(rhs.in() ? 'r' : '-'),
		(rhs.out() ? 'w' : '-'),
		(rhs.hup() ? 'c' : '-'),
		(rhs.err() ? 'e' : '-'),
		0
	};
	const char ev[] = {
		((rhs.events() & poll_event::In) ? 'r' : '-'),
		((rhs.events() & poll_event::Out) ? 'w' : '-'),
		0
	};
	return out << make_object("fd", rhs.fd(), "rev", rev, "ev", ev);
}

