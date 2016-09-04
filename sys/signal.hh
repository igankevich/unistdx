#ifndef SYS_SIGNAL_HH
#define SYS_SIGNAL_HH

#include <signal.h>

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
	operator<<(std::ostream& out, const signal rhs) {
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
			#ifdef SIGPOLL
			case signal::poll: out << "poll"; break;
			#endif
			case signal::profile: out << "profile"; break;
			case signal::bad_argument: out << "bad_argument"; break;
			case signal::breakpoint: out << "breakpoint"; break;
			case signal::urgent: out << "urgent"; break;
			case signal::virtual_alarm: out << "virtual_alarm"; break;
			case signal::cpu_time_limit_exceeded: out << "cpu_time_limit_exceeded"; break;
			case signal::file_size_limit_exceeded: out << "file_size_limit_exceeded"; break;
			#ifdef SIGSTKFLT
			case signal::coprocessor_stack_fault: out << "coprocessor_stack_fault"; break;
			#endif
			#ifdef SIGPWR
			case signal::power_failure: out << "power_failure"; break;
			#endif
			#ifdef SIGWINCH
			case signal::window_resize: out << "window_resize"; break;
			#endif
		}
		out << '(' << signal_type(rhs) << ')';
		return out;
	}

	struct signal_action: public sigaction_type {
		signal_action() = default;
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
