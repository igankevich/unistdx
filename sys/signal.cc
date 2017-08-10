#include "signal.hh"

std::ostream&
sys::operator<<(std::ostream& out, const signal rhs) {
	switch (rhs) {
		case signal::hang_up: out << "hang_up"; break;
		case signal::keyboard_interrupt: out << "keyboard_interrupt"; break;
		case signal::quit: out << "quit"; break;
		case signal::illegal_instruction: out << "illegal_instruction"; break;
		case signal::abort: out << "abort"; break;
		case signal::floating_point_exception: out << "floating_point_exception"; break;
		case signal::kill: out << "kill"; break;
		case signal::segmentation_fault: out << "segmentation_fault"; break;
		case signal::broken_pipe: out << "broken_pipe"; break;
		case signal::alarm: out << "alarm"; break;
		case signal::terminate: out << "terminate"; break;
		case signal::user_defined_1: out << "user_defined_1"; break;
		case signal::user_defined_2: out << "user_defined_2"; break;
		case signal::child: out << "child"; break;
		case signal::resume: out << "resume"; break;
		case signal::stop: out << "stop"; break;
		case signal::stop_from_terminal: out << "stop_from_terminal"; break;
		case signal::terminal_input: out << "terminal_input"; break;
		case signal::terminal_output: out << "terminal_output"; break;
		case signal::bad_memory_access: out << "bad_memory_access"; break;
		#if defined(UNISTDX_HAVE_SIGPOLL)
		case signal::poll: out << "poll"; break;
		#endif
		case signal::profile: out << "profile"; break;
		case signal::bad_argument: out << "bad_argument"; break;
		case signal::breakpoint: out << "breakpoint"; break;
		case signal::urgent: out << "urgent"; break;
		case signal::virtual_alarm: out << "virtual_alarm"; break;
		case signal::cpu_time_limit_exceeded: out << "cpu_time_limit_exceeded"; break;
		case signal::file_size_limit_exceeded: out << "file_size_limit_exceeded"; break;
		#if defined(UNISTDX_HAVE_SIGSTKFLT)
		case signal::coprocessor_stack_fault: out << "coprocessor_stack_fault"; break;
		#endif
		#if defined(UNISTDX_HAVE_SIGPWR)
		case signal::power_failure: out << "power_failure"; break;
		#endif
		#if defined(UNISTDX_HAVE_SIGWINCH)
		case signal::window_resize: out << "window_resize"; break;
		#endif
	}
	out << '(' << signal_type(rhs) << ')';
	return out;
}

