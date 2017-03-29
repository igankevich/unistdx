#ifndef SYS_SIGNAL_HH
#define SYS_SIGNAL_HH

#include <signal.h>
#include <ostream>
#include "bits/check.hh"

namespace sys {

	typedef int signal_type;
	typedef struct ::sigaction sigaction_type;

	enum struct signal: signal_type {
		// POSIX.1-1990
		hang_up = SIGHUP,
		keyboard_interrupt = SIGINT,
		quit = SIGQUIT,
		illegal_instruction = SIGILL,
		abort = SIGABRT,
		floating_point_exception = SIGFPE,
		kill = SIGKILL,
		segmentation_fault = SIGSEGV,
		broken_pipe = SIGPIPE,
		alarm = SIGALRM,
		terminate = SIGTERM,
		user_defined_1 = SIGUSR1,
		user_defined_2 = SIGUSR2,
		child = SIGCHLD,
		resume = SIGCONT,
		stop = SIGSTOP,
		stop_from_terminal = SIGTSTP,
		terminal_input = SIGTTIN,
		terminal_output = SIGTTOU,
		// POSIX.1-2001
		bad_memory_access = SIGBUS,
		#ifdef SIGPOLL
		poll = SIGPOLL,
		#endif
		profile = SIGPROF,
		bad_argument = SIGSYS,
		breakpoint = SIGTRAP,
		urgent = SIGURG,
		virtual_alarm = SIGVTALRM,
		cpu_time_limit_exceeded = SIGXCPU,
		file_size_limit_exceeded = SIGXFSZ
		// non-standard
		#ifdef SIGSTKFLT
		, coprocessor_stack_fault = SIGSTKFLT
		#endif
		#ifdef SIGPWR
		, power_failure = SIGPWR
		#endif
		#ifdef SIGWINCH
		, window_resize = SIGWINCH
		#endif
	};

	std::ostream&
	operator<<(std::ostream& out, const signal rhs);

	struct signal_action: public sigaction_type {

		inline
		signal_action() = default;

		inline
		signal_action(void (*func)(int)) noexcept {
			this->sa_handler = func;
		}
	};

	namespace this_process {

		inline void
		bind_signal(signal sig, const signal_action& action) {
			bits::check(
				::sigaction(signal_type(sig), &action, 0),
				__FILE__, __LINE__, __func__
			);
		}

		inline void
		ignore_signal(signal sig) {
			bind_signal(sig, SIG_IGN);
		}

	}

}

#endif // SYS_SIGNAL_HH
