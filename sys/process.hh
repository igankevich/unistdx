#ifndef SYS_PROCESS_HH
#define SYS_PROCESS_HH

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <algorithm>
#include <vector>

#include <stdx/iterator.hh>

#include <sys/bits/check.hh>
#include <sys/bits/safe_calls.hh>

#include "signal.hh"
#include "argstream.hh"

namespace sys {

	typedef ::pid_t pid_type;
	typedef int wstat_type;
	typedef int code_type;
	typedef ::siginfo_t siginfo_type;
	typedef ::uid_t uid_type;
	typedef ::gid_t gid_type;

	constexpr uid_type
	superuser() noexcept { return 0; }

	constexpr gid_type
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
			bits::check(::setpgid(this_process::id(), rhs),
				__FILE__, __LINE__, __func__);
		}
		*/

		inline uid_type
		user() noexcept { return ::getuid(); }

		inline void
		set_user(uid_type rhs) {
			bits::check(
				::setuid(rhs),
				__FILE__, __LINE__, __func__
			);
		}

		inline uid_type
		effective_user() noexcept { return ::geteuid(); }

		inline gid_type
		group() noexcept { return ::getgid(); }

		inline void
		set_group(gid_type rhs) {
			bits::check(
				::setgid(rhs),
				__FILE__, __LINE__, __func__
			);
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
			return bits::check(
				::execve(argv[0], argv, no_env),
				__FILE__, __LINE__, __func__
			);
		}

		int
		execute(char* const argv[]) {
			char* const no_env[1] = {0};
			return bits::check(
				::execve(argv[0], argv, no_env),
				__FILE__, __LINE__, __func__
			);
		}

		template<class ... Args>
		int
		execute_command(const Args& ... args) {
			sys::argstream str;
			str.append(args...);
			assert(str.argc() == sizeof...(Args));
			char** argv = str.argv();
			return bits::check(
				::execvp(argv[0], argv),
				__FILE__, __LINE__, __func__
			);
		}

		int
		execute_command(char* const argv[]) {
			return bits::check(
				::execvp(argv[0], argv),
				__FILE__, __LINE__, __func__
			);
		}

		void
		send(signal sig) {
			bits::check(
				::kill(::sys::this_process::id(), signal_type(sig)),
				__FILE__, __LINE__, __func__
			);
		}

	}

	pid_type
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

		explicit constexpr
		basic_status(wstat_type rhs) noexcept:
			stat(rhs) {}

		constexpr
		basic_status() noexcept = default;

		constexpr
		basic_status(const basic_status&) noexcept = default;

		constexpr bool
		exited() const noexcept {
			return WIFEXITED(stat);
		}

		constexpr bool
		killed() const noexcept {
			return WIFSIGNALED(stat);
		}

		constexpr bool
		stopped() const noexcept {
			return WIFSTOPPED(stat);
		}

		constexpr bool
		core_dumped() const noexcept {
			#ifdef WCOREDUMP
			return static_cast<bool>(WCOREDUMP(stat));
			#else
			return false;
			#endif
		}

		constexpr bool
		trapped() const noexcept {
			return false;
		}

		constexpr bool
		continued() const noexcept {
			return WIFCONTINUED(stat);
		}

		constexpr code_type
		exit_code() const noexcept {
			return WEXITSTATUS(stat);
		}

		constexpr signal
		term_signal() const noexcept {
			return signal(WTERMSIG(stat));
		}

		constexpr signal
		stop_signal() const noexcept {
			return signal(WSTOPSIG(stat));
		}

		friend std::ostream&
		operator<<(std::ostream& out, const basic_status& rhs) {
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

		explicit constexpr
		basic_status(const siginfo_type& rhs) noexcept:
			stat(static_cast<st>(rhs.si_code)),
			code(rhs.si_status),
			_pid(rhs.si_pid) {}

		constexpr
		basic_status() noexcept = default;

		constexpr
		basic_status(const basic_status&) noexcept = default;

		constexpr bool
		exited() const noexcept {
			return stat == st::exited;
		}

		constexpr bool
		killed() const noexcept {
			return stat == st::killed;
		}

		constexpr bool
		stopped() const noexcept {
			return stat == st::stopped;
		}

		constexpr bool
		core_dumped() const noexcept {
			return stat == st::core_dumped;
		}

		constexpr bool
		trapped() const noexcept {
			return stat == st::trapped;
		}

		constexpr bool
		continued() const noexcept {
			return stat == st::continued;
		}

		constexpr code_type
		exit_code() const noexcept {
			return code;
		}

		constexpr signal
		term_signal() const noexcept {
			return signal(code);
		}

		constexpr signal
		stop_signal() const noexcept {
			return signal(code);
		}

		constexpr pid_type
		pid() const noexcept {
			return _pid;
		}

		constexpr const char*
		status_string() const noexcept {
			return exited() ? "exited" :
				killed() ? "killed" :
				core_dumped() ? "core_dumped" :
				stopped() ? "stopped" :
				continued() ? "continued" :
				trapped() ? "trapped" :
				"unknown";
		}

		friend std::ostream&
		operator<<(std::ostream& out, const basic_status& rhs) {
			out << "pid=" << rhs.pid() << ',';
			out << "status=" << rhs.status_string() << ',';
			if (rhs.exited()) {
				out << "exit_code=" << rhs.exit_code();
			} else {
				out << "signal=" << rhs.term_signal();
			}
			return out;
		}

		st stat = static_cast<st>(0);
		code_type code = 0;
		pid_type _pid = 0;
	};

	typedef basic_status<wstat_type> proc_status;
	typedef basic_status<siginfo_type> proc_info;

	struct process {

		template<class F>
		explicit inline
		process(F f) {
			_pid = safe_fork();
			if (_pid == 0) {
				int ret = f();
				std::exit(ret);
			}
		}

		explicit
		process(pid_type rhs) noexcept:
		_pid(rhs)
		{}

		process() = default;
		process(const process&) = delete;

		inline
		process(process&& rhs) noexcept:
		_pid(rhs._pid)
		{
			rhs._pid = 0;
		}

		~process() {
			if (_pid > 0) {
				this->do_kill(sys::signal::terminate);
			}
		}

		process&
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
			if (_pid > 0) {
		    	bits::check(do_kill(sig),
					__FILE__, __LINE__, __func__);
			}
		}

		sys::proc_status
		wait() {
			int stat = 0;
			if (_pid > 0) {
				bits::check_if_not<std::errc::interrupted>(
					::waitpid(_pid, &stat, 0),
					__FILE__, __LINE__, __func__);
				_pid = 0;
			}
			return sys::proc_status(stat);
		}

		explicit inline
		operator bool() const noexcept {
			return _pid > 0 && do_kill(sys::signal(0)) != -1;
		}

		inline bool
		operator !() const noexcept {
			return !operator bool();
		}

		friend std::ostream&
		operator<<(std::ostream& out, const process& rhs) {
			return out << stdx::make_object("id", rhs.id(), "gid", rhs.group_id());
		}

		pid_type
		id() const noexcept {
			return _pid;
		}

		pid_type
		group_id() const noexcept {
			return ::getpgid(_pid);
		}

		void
		set_group_id(pid_type rhs) const {
			bits::check(::setpgid(_pid, rhs),
				__FILE__, __LINE__, __func__);
		}

		/// std::thread interface
		bool joinable() { return true; }
		void join() { wait(); }

	private:

		inline int
		do_kill(sys::signal sig) const noexcept {
		   	return ::kill(_pid, signal_type(sig));
		}

		pid_type _pid = 0;

	};

	struct process_group {

		typedef std::vector<process>::iterator iterator;

		template<class ... Args>
		explicit
		process_group(Args&& ... args) {
			emplace(std::forward<Args>(args)...);
		}

		process_group() = default;
		process_group(const process_group&) = delete;
		process_group(process_group&&) = default;
		~process_group() = default;

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

		int
		wait() {
			int ret = 0;
			for (process& p : _procs) {
				sys::proc_status x = p.wait();
				#ifndef NDEBUG
				stdx::debug_message("sys", "process _ terminated with status _", p, x);
				#endif
				ret |= x.exit_code() | signal_type(x.term_signal());
			}
			return ret;
		}

		template<class F>
		void
		wait(F callback, wait_flags flags=proc_exited) {
			while (!_procs.empty()) {
				sys::proc_info status = do_wait(flags);
				auto result = std::find_if(
					_procs.begin(), _procs.end(),
					[&status] (const process& p) {
						return p.id() == status.pid();
					}
				);
				if (result != _procs.end()) {
					callback(*result, status);
					_procs.erase(result);
				}
			}
		}

		inline void
		terminate() {
			if (_gid > 0) {
		    	bits::check(::kill(_gid, SIGTERM),
					__FILE__, __LINE__, __func__);
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

		process&
		operator[](size_t i) noexcept {
			return _procs[i];
		}

		size_t
		size() const noexcept {
			return _procs.size();
		}

		iterator
		begin() noexcept {
			return _procs.begin();
		}

		iterator
		end() noexcept {
			return _procs.end();
		}

		const process&
		front() const noexcept {
			return _procs.front();
		}

		process&
		front() noexcept {
			return _procs.front();
		}

		const process&
		back() const noexcept {
			return _procs.back();
		}

		process&
		back() noexcept {
			return _procs.back();
		}

		friend std::ostream&
		operator<<(std::ostream& out, const process_group& rhs) {
			out << "{gid=" << rhs._gid << ",[";
			std::copy(rhs._procs.begin(), rhs._procs.end(),
				stdx::intersperse_iterator<process>(out, ","));
			out << "]}";
			return out;
		}

	private:

		sys::proc_info
		do_wait(wait_flags flags) const {
			sys::siginfo_type info;
			bits::check_if_not<std::errc::interrupted>(
				::waitid(P_PGID, _gid, &info, flags),
				__FILE__, __LINE__, __func__);
			return sys::proc_info(info);
		}

		std::vector<process> _procs;
		pid_type _gid = 0;
	};

}

#endif // SYS_PROCESS_HH
