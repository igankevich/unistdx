#include "signal.hh"

namespace {

	constexpr const int nsignals = sizeof(::sigset_t);

	struct init_signal_names {

		const char* signal_names[nsignals] = {0};

		init_signal_names() {
			signal_names[SIGHUP] = "hang_up";
			signal_names[SIGINT] = "keyboard_interrupt";
			signal_names[SIGQUIT] = "quit";
			signal_names[SIGILL] = "illegal_instruction";
			signal_names[SIGABRT] = "abort";
			signal_names[SIGFPE] = "floating_point_exception";
			signal_names[SIGKILL] = "kill";
			signal_names[SIGSEGV] = "segmentation_fault";
			signal_names[SIGPIPE] = "broken_pipe";
			signal_names[SIGALRM] = "alarm";
			signal_names[SIGTERM] = "terminate";
			signal_names[SIGUSR1] = "user_defined_1";
			signal_names[SIGUSR2] = "user_defined_2";
			signal_names[SIGCHLD] = "child";
			signal_names[SIGCONT] = "resume";
			signal_names[SIGSTOP] = "stop";
			signal_names[SIGTSTP] = "stop_from_terminal";
			signal_names[SIGTTIN] = "terminal_input";
			signal_names[SIGTTOU] = "terminal_output";
			signal_names[SIGBUS] = "bad_memory_access";
			#if defined(UNISTDX_HAVE_SIGPOLL)
			signal_names[SIGPOLL] = "poll";
			#endif
			signal_names[SIGPROF] = "profile";
			signal_names[SIGSYS] = "bad_argument";
			signal_names[SIGTRAP] = "breakpoint";
			signal_names[SIGURG] = "urgent";
			signal_names[SIGVTALRM] = "virtual_alarm";
			signal_names[SIGXCPU] = "cpu_time_limit_exceeded";
			signal_names[SIGXFSZ] ="file_size_limit_exceeded";
			#if defined(UNISTDX_HAVE_SIGSTKFLT)
			signal_names[SIGSTKFLT] = "coprocessor_stack_fault";
			#endif
			#if defined(UNISTDX_HAVE_SIGPWR)
			signal_names[SIGPWR] = "power_failure";
			#endif
			#if defined(UNISTDX_HAVE_SIGWINCH)
			signal_names[SIGWINCH] ="window_resize";
			#endif
		}

		inline const char*
		operator[](sys::signal_type s) {
			return this->signal_names[s];
		}

	} signal_names;

}

std::ostream&
sys::operator<<(std::ostream& out, const signal rhs) {
	const signal_type s = signal_type(rhs);
	const char* name = (s >= 0 && s < nsignals) ? signal_names[s] : nullptr;
	out << (name ? name : "unknown") << '(' << s << ')';
	return out;
}
