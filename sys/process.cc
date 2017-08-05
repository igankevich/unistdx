#include <stdx/debug.hh>
#include "process.hh"

#include <algorithm>

std::ostream&
sys::operator<<(std::ostream& out, const proc_status& rhs) {
	if (rhs.exited()) {
		out << stdx::make_object("status", "exited", "exit_code", rhs.exit_code());
	} else
	if (rhs.killed()) {
		out << stdx::make_object("status", "killed", "term_signal", rhs.term_signal());
	} else
	if (rhs.stopped()) {
		out << stdx::make_object("status", "stopped", "stop_signal", rhs.stop_signal());
	} else
	if (rhs.continued()) {
		out << stdx::make_object("status", "continued");
	} else {
		out << stdx::make_object("status", "unknown");
	}
	return out;
}

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

int
sys::process_group::wait() {
	int ret = 0;
	for (process& p : this->_procs) {
		sys::proc_status x = p.wait();
		#ifndef NDEBUG
		stdx::debug_message("sys", "process _ terminated with status _", p, x);
		#endif
		ret |= x.exit_code() | signal_type(x.term_signal());
	}
	return ret;
}

void
sys::process_group::do_wait(
	wait_flags flags,
	sys::proc_info& status,
	const_iterator& result
) const {
	sys::siginfo_type info;
	bits::check_if_not<std::errc::interrupted>(
		::waitid(P_PGID, this->_gid, &info, flags),
		__FILE__, __LINE__, __func__);
	status = sys::proc_info(info);
	result = std::find_if(
		_procs.begin(), _procs.end(),
		[&status] (const process& p) {
			return p.id() == status.pid();
		}
	);
}

std::ostream&
sys::operator<<(std::ostream& out, const sys::process_group& rhs) {
	out << "{gid=" << rhs._gid << ",[";
	std::copy(rhs._procs.begin(), rhs._procs.end(),
		stdx::intersperse_iterator<process>(out, ","));
	out << "]}";
	return out;
}

