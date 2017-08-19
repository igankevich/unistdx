#include "process_group"
#include <unistdx/base/log_message>
#include <unistdx/it/intersperse_iterator>
#include <algorithm>

int
sys::process_group::wait() {
	int ret = 0;
	for (process& p : this->_procs) {
		sys::proc_status x = p.wait();
		#ifndef NDEBUG
		log_message("sys", "process _ terminated with status _", p, x);
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
	UNISTDX_CHECK_IF_NOT(EINTR, ::waitid(P_PGID, this->_gid, &info, flags));
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
		intersperse_iterator<process>(out, ","));
	out << "]}";
	return out;
}


