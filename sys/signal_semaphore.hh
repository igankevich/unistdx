#ifndef SYS_SIGNAL_SEMAPHORE_HH
#define SYS_SIGNAL_SEMAPHORE_HH

namespace sys {

	struct sigset_type: public ::sigset_t {

		sigset_type() {
			UNISTDX_CHECK(::sigemptyset(this));
		}

		explicit
		sigset_type(signal_type s):
		sigset_type()
		{
			UNISTDX_CHECK(::sigaddset(this, s));
		}

		template<class F>
		void for_each(F func) {
			for (signal_type s=1; s<=31; ++s) {
				int ret;
				UNISTDX_CHECK(ret = ::sigismember(this, s));
				if (ret) {
					func(s);
				}
			}
		}

	};

	struct signal_lock {

		explicit
		signal_lock(sigset_type s):
		_signals(s)
		{ lock(); }

		~signal_lock() {
			unlock();
		}

		void lock() {
			change_mask(SIG_BLOCK);
		}

		void unlock() {
			change_mask(SIG_UNBLOCK);
		}

	private:

		void change_mask(int how) {
			UNISTDX_CHECK(::sigprocmask(how, &_signals, 0));
		}

		sigset_type _signals;
	};

	struct signal_semaphore {

		typedef siginfo_t siginfo_type;
		typedef union ::sigval payload_type;

		explicit
		signal_semaphore(pid_type owner, signal_type s):
		_owner(owner), _sig(s), _lastnotifier(0)
		{}

		void
		notify_one() {
			// notifying non-existent process is not an error
			bits::check_if_not<std::errc::no_such_process>(
				::kill(_owner, _sig),
				__FILE__, __LINE__, __func__);
		}

		void
		notify_one(payload_type payload) {
			bits::check_if_not<std::errc::no_such_process>(
				::sigqueue(_owner, _sig, payload),
				__FILE__, __LINE__, __func__);
		}

		void
		notify_all() {
			notify_one();
		}

		void wait() {
			sigset_type sigs(_sig);
			signal_lock lock(sigs);
			siginfo_type result;
			std::cerr << "Waiting for signal sig=" << _sig << std::endl;
			bits::check_if_not<std::errc::interrupted>(
				::sigwaitinfo(&sigs, &result),
				__FILE__, __LINE__, __func__);
			if (std::errc(errno) != std::errc::interrupted) {
				_lastnotifier = result.si_pid;
				if (result.si_code == SI_QUEUE) {
					std::clog << "Received sival=" << result.si_value.sival_int << std::endl;
					_queue.push(result.si_value);
				}
			}
		}

		template<class Lock>
		void wait(Lock& lock) {
			stdx::unlock_guard<Lock> unlock(lock);
			wait();
		}

		template<class Lock, class Pred>
		void wait(Lock& lock, Pred pred) {
			while (!pred()) {
				wait(lock);
			}
		}

		pid_type
		last_notifier() {
			return _lastnotifier;
		}

		const std::queue<payload_type>&
		payload_queue() const {
			return _queue;
		}

		std::queue<payload_type>&
		payload_queue() {
			return _queue;
		}

	private:

		pid_type _owner;
		signal_type _sig;
		pid_type _lastnotifier;
		std::queue<payload_type> _queue;
	};

	struct init_signal_semaphore {

		/// Block signals from set @s in all threads
		/// (this object should be constructed
		/// before launching any threads, so
		/// that they inherit sigmask from
		/// the main thread).
		explicit
		init_signal_semaphore(sigset_type s):
		_lock(s)
		{
			install_empty_signal_handler(s);
		}

		explicit
		init_signal_semaphore(signal_type s):
		init_signal_semaphore(sigset_type(s))
		{}

	private:

		/// install empty signal handler
		/// to make signal queueable,
		/// otherwise it kills receiver process
		void
		install_empty_signal_handler(sigset_type s) {
			signal_action action{};
			action.sa_handler = &init_signal_semaphore::no_handler;
			action.sa_flags = SA_SIGINFO;
			action.sa_mask = sigset_type{};
			s.for_each([this,&action](signal_type sig) {
				this_process::bind_signal(signal(sig), action);
			});
		}

		static void no_handler(int) {}

		/// block/unblock signal on construction/destruction
		signal_lock _lock;
	};

}

#endif // SYS_SIGNAL_SEMAPHORE_HH
