#ifndef SYS_PROCESS_HH
#define SYS_PROCESS_HH

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <algorithm>
#include <vector>

#include <stdx/iterator.hh>

#include <sys/check>
#include <sys/bits/safe_calls.hh>

#include "signal"
#include "argstream.hh"

namespace sys {

	typedef ::pid_t pid_type;
	typedef int wstat_type;
	typedef int code_type;
	typedef ::siginfo_t siginfo_type;
	typedef ::uid_t uid_type;
	typedef ::gid_t gid_type;

	inline constexpr uid_type
	superuser() noexcept { return 0; }

	inline constexpr gid_type
	supergroup() noexcept { return 0; }

	namespace this_process {

		inline pid_type
		id() noexcept { return ::getpid(); }

		inline pid_type
		parent_id() noexcept { return ::getppid(); }

		/*
		inline pid_type
		group_id() noexcept { return ::getpgrp(); }

		inline void
		set_group_id(pid_type rhs) {
			UNISTDX_CHECK(::setpgid(this_process::id(), rhs));
		}
		*/

		inline uid_type
		user() noexcept { return ::getuid(); }

		inline void
		set_user(uid_type rhs) {
			UNISTDX_CHECK(::setuid(rhs));
		}

		inline uid_type
		effective_user() noexcept { return ::geteuid(); }

		inline gid_type
		group() noexcept { return ::getgid(); }

		inline void
		set_group(gid_type rhs) {
			UNISTDX_CHECK(::setgid(rhs));
		}

		inline gid_type
		effective_group() noexcept { return ::getegid(); }

		template<class ... Args>
		int
		execute(const Args& ... args) {
			sys::argstream str;
			str.append(args...);
			assert(str.argc() == sizeof...(Args));
			char** argv = str.argv();
			char* const no_env[1] = {0};
			int ret;
			UNISTDX_CHECK(ret = ::execve(argv[0], argv, no_env));
			return ret;
		}

		inline int
		execute(char* const argv[]) {
			char* const no_env[1] = {0};
			int ret;
			UNISTDX_CHECK(ret = ::execve(argv[0], argv, no_env));
			return ret;
		}

		template<class ... Args>
		int
		execute_command(const Args& ... args) {
			sys::argstream str;
			str.append(args...);
			assert(str.argc() == sizeof...(Args));
			char** argv = str.argv();
			int ret;
			UNISTDX_CHECK(ret = ::execvp(argv[0], argv));
			return ret;
		}

		inline int
		execute_command(char* const argv[]) {
			int ret;
			UNISTDX_CHECK(ret = ::execvp(argv[0], argv));
			return ret;
		}

		inline void
		send(signal sig) {
			UNISTDX_CHECK(::kill(::sys::this_process::id(), signal_type(sig)));
		}

	}

	inline pid_type
	safe_fork() {
		bits::global_lock_type lock(bits::__forkmutex);
		return ::fork();
	}

	enum wait_flags {
		proc_exited = WEXITED,
		proc_stopped = WSTOPPED,
		proc_continued = WCONTINUED
	};

	template<class T>
	struct basic_status {};

	template<>
	struct basic_status<wstat_type> {

		inline explicit constexpr
		basic_status(wstat_type rhs) noexcept:
			stat(rhs) {}

		inline constexpr
		basic_status() noexcept = default;

		inline constexpr
		basic_status(const basic_status&) noexcept = default;

		inline constexpr bool
		exited() const noexcept {
			return WIFEXITED(stat);
		}

		inline constexpr bool
		killed() const noexcept {
			return WIFSIGNALED(stat);
		}

		inline constexpr bool
		stopped() const noexcept {
			return WIFSTOPPED(stat);
		}

		inline constexpr bool
		core_dumped() const noexcept {
			#if defined(WCOREDUMP)
			return static_cast<bool>(WCOREDUMP(stat));
			#else
			return false;
			#endif
		}

		inline constexpr bool
		trapped() const noexcept {
			return false;
		}

		inline constexpr bool
		continued() const noexcept {
			return WIFCONTINUED(stat);
		}

		inline constexpr code_type
		exit_code() const noexcept {
			return WEXITSTATUS(stat);
		}

		inline constexpr signal
		term_signal() const noexcept {
			return signal(WTERMSIG(stat));
		}

		inline constexpr signal
		stop_signal() const noexcept {
			return signal(WSTOPSIG(stat));
		}

		wstat_type stat = 0;
	};

	template<>
	struct basic_status<siginfo_type> {

		enum struct st {
			exited = CLD_EXITED,
			killed = CLD_KILLED,
			core_dumped = CLD_DUMPED,
			stopped = CLD_STOPPED,
			trapped = CLD_TRAPPED,
			continued = CLD_CONTINUED
		};

		inline explicit constexpr
		basic_status(const siginfo_type& rhs) noexcept:
			stat(static_cast<st>(rhs.si_code)),
			code(rhs.si_status),
			_pid(rhs.si_pid) {}

		inline constexpr
		basic_status() noexcept = default;

		inline constexpr
		basic_status(const basic_status&) noexcept = default;

		inline constexpr bool
		exited() const noexcept {
			return stat == st::exited;
		}

		inline constexpr bool
		killed() const noexcept {
			return stat == st::killed;
		}

		inline constexpr bool
		stopped() const noexcept {
			return stat == st::stopped;
		}

		inline constexpr bool
		core_dumped() const noexcept {
			return stat == st::core_dumped;
		}

		inline constexpr bool
		trapped() const noexcept {
			return stat == st::trapped;
		}

		inline constexpr bool
		continued() const noexcept {
			return stat == st::continued;
		}

		inline constexpr code_type
		exit_code() const noexcept {
			return code;
		}

		inline constexpr signal
		term_signal() const noexcept {
			return signal(code);
		}

		inline constexpr signal
		stop_signal() const noexcept {
			return signal(code);
		}

		inline constexpr pid_type
		pid() const noexcept {
			return _pid;
		}

		inline constexpr const char*
		status_string() const noexcept {
			return exited() ? "exited" :
				killed() ? "killed" :
				core_dumped() ? "core_dumped" :
				stopped() ? "stopped" :
				continued() ? "continued" :
				trapped() ? "trapped" :
				"unknown";
		}

		st stat = static_cast<st>(0);
		code_type code = 0;
		pid_type _pid = 0;
	};

	typedef basic_status<wstat_type> proc_status;
	typedef basic_status<siginfo_type> proc_info;

	std::ostream&
	operator<<(std::ostream& out, const proc_status& rhs);

	std::ostream&
	operator<<(std::ostream& out, const proc_info& rhs);

	struct process {

		template<class F>
		inline explicit
		process(F f) {
			_pid = safe_fork();
			if (_pid == 0) {
				int ret = f();
				std::exit(ret);
			}
		}

		inline explicit
		process(pid_type rhs) noexcept:
		_pid(rhs)
		{}

		inline process() = default;
		process(const process&) = delete;

		inline
		process(process&& rhs) noexcept:
		_pid(rhs._pid)
		{
			rhs._pid = 0;
		}

		inline
		~process() {
			if (_pid > 0) {
				this->do_kill(sys::signal::terminate);
			}
		}

		inline process&
		operator=(process&& rhs) noexcept {
			std::swap(_pid, rhs._pid);
			return *this;
		}

		inline void terminate() { this->send(sys::signal::terminate); }
		inline void kill() { this->send(sys::signal::kill); }
		inline void interrupt() { this->send(sys::signal::keyboard_interrupt); }
		inline void hang_up() { this->send(sys::signal::hang_up); }

		inline void
		send(sys::signal sig) {
			if (this->_pid > 0) {
		    	UNISTDX_CHECK(do_kill(sig));
			}
		}

		inline sys::proc_status
		wait() {
			int stat = 0;
			if (this->_pid > 0) {
				UNISTDX_CHECK_IF_NOT(EINTR, ::waitpid(_pid, &stat, 0));
				this->_pid = 0;
			}
			return sys::proc_status(stat);
		}

		inline explicit
		operator bool() const noexcept {
			return _pid > 0 && do_kill(sys::signal(0)) != -1;
		}

		inline bool
		operator !() const noexcept {
			return !operator bool();
		}

		friend std::ostream&
		operator<<(std::ostream& out, const process& rhs);

		inline pid_type
		id() const noexcept {
			return _pid;
		}

		inline pid_type
		group_id() const noexcept {
			return ::getpgid(_pid);
		}

		inline void
		set_group_id(pid_type rhs) const {
			UNISTDX_CHECK(::setpgid(_pid, rhs));
		}

		/// std::thread interface
		inline bool joinable() { return true; }
		inline void join() { wait(); }

	private:

		inline int
		do_kill(sys::signal sig) const noexcept {
		   	return ::kill(_pid, signal_type(sig));
		}

		pid_type _pid = 0;

	};

	std::ostream&
	operator<<(std::ostream& out, const process& rhs);

	struct process_group {

		typedef std::vector<process>::iterator iterator;
		typedef std::vector<process>::const_iterator const_iterator;

		template<class ... Args>
		inline explicit
		process_group(Args&& ... args) {
			emplace(std::forward<Args>(args)...);
		}

		inline process_group() = default;
		process_group(const process_group&) = delete;
		inline process_group(process_group&&) = default;
		inline ~process_group() = default;

		template<class F>
		const process&
		emplace(F&& childmain) {
			_procs.emplace_back(std::forward<F>(childmain));
			process& proc = _procs.back();
			if (_procs.size() == 1) {
				_gid = proc.id();
			}
			proc.set_group_id(_gid);
			return proc;
		}

		template<class Func, class ... Args>
		void
		emplace(Func&& main, Args&& ... args) {
			this->emplace(std::forward<Func>(main));
			this->emplace(std::forward<Args>(args)...);
		}

		int wait();

		template<class F>
		void
		wait(F callback, wait_flags flags=proc_exited) {
			while (!_procs.empty()) {
				sys::proc_info status;
				const_iterator result;
				do_wait(flags, status, result);
				if (result != _procs.end()) {
					callback(*result, status);
					_procs.erase(result);
				}
			}
		}

		inline void
		terminate() {
			if (this->_gid > 0) {
		    	UNISTDX_CHECK(::kill(_gid, SIGTERM));
			}
		}

		inline pid_type
		id() const noexcept {
			return _gid;
		}

		const process&
		operator[](size_t i) const noexcept {
			return _procs[i];
		}

		inline process&
		operator[](size_t i) noexcept {
			return _procs[i];
		}

		inline size_t
		size() const noexcept {
			return _procs.size();
		}

		inline iterator
		begin() noexcept {
			return _procs.begin();
		}

		inline iterator
		end() noexcept {
			return _procs.end();
		}

		inline const process&
		front() const noexcept {
			return _procs.front();
		}

		inline process&
		front() noexcept {
			return _procs.front();
		}

		inline const process&
		back() const noexcept {
			return _procs.back();
		}

		inline process&
		back() noexcept {
			return _procs.back();
		}

		friend std::ostream&
		operator<<(std::ostream& out, const process_group& rhs);

	private:

		void
		do_wait(
			wait_flags flags,
			sys::proc_info& status,
			const_iterator& result
		) const;

		std::vector<process> _procs;
		pid_type _gid = 0;
	};

	std::ostream&
	operator<<(std::ostream& out, const process_group& rhs);

}

#endif // SYS_PROCESS_HH
