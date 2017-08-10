#ifndef SYS_SEMAPHORE_HH
#define SYS_SEMAPHORE_HH

#include <unistd.h>
#include <signal.h>

#include <unistdx_config>
#if defined(UNISTDX_HAVE_POSIX_SEMAPHORES)
#include <semaphore.h>
#endif
#if defined(UNISTDX_HAVE_SYSV_SEMAPHORES)
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

#include <condition_variable>
#include <cassert>

#include <stdx/mutex.hh>

#include <sys/fildes.hh>
#include <sys/process.hh>

namespace sys {

	#if defined(UNISTDX_HAVE_SYSV_SEMAPHORES)
	struct sysv_semaphore {

		typedef int sem_type;
		typedef struct ::sembuf sembuf_type;

		inline explicit
		sysv_semaphore(mode_type mode=0600):
		_owner(true)
		{
			UNISTDX_CHECK(
				this->_sem = ::semget(IPC_PRIVATE, _nsems, IPC_CREAT|mode)
			);
		}

		inline explicit
		sysv_semaphore(sem_type sem):
		_sem(sem),
		_owner(false)
		{}

		inline
		~sysv_semaphore() {
			if (this->_owner) {
				(void)::semctl(this->_sem, IPC_RMID, 0);
			}
		}

		inline sem_type
		id() const noexcept {
			return this->_sem;
		}

		inline void
		wait() {
			this->increment(-1);
		}

		inline void
		notify_one() {
			this->increment(1);
		}

		inline void
		notify_all() {
			this->notify_all(1000);
		}

		inline void
		notify_all(int n) {
			this->increment(n);
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

	private:

		inline void
		increment(int how) {
			sembuf_type buf;
			buf.sem_num = 0;
			buf.sem_op = how;
			buf.sem_flg = SEM_UNDO;
			UNISTDX_CHECK(::semop(_sem, &buf, _nsems));
		}

		sem_type _sem;
		bool _owner;

		static const int _nsems = 1;
	};
	#endif

	#if defined(UNISTDX_HAVE_POSIX_SEMAPHORES)
	struct process_semaphore {

		typedef ::sem_t* sem_type;
		typedef int flags_type;
		typedef std::string path_type;

		process_semaphore() = default;

		inline explicit
		process_semaphore(path_type&& name, mode_type mode):
		_path(std::forward<path_type>(name)),
		_owner(true),
		_sem(this->open_sem(mode))
		{}

		inline explicit
		process_semaphore(path_type&& name):
		_path(std::forward<path_type>(name)),
		_owner(false),
		_sem(this->open_sem())
		{}

		inline
		process_semaphore(process_semaphore&& rhs) noexcept:
		_path(std::move(rhs._path)),
		_owner(rhs._owner),
		_sem(rhs._sem)
		{ rhs._sem = SEM_FAILED; }

		inline
		~process_semaphore() {
			this->close();
		}

		process_semaphore(const process_semaphore&) = delete;
		process_semaphore& operator=(const process_semaphore&) = delete;

		inline void
		open(path_type&& name, mode_type mode) {
			this->close();
			this->_path = std::forward<path_type>(name),
			this->_owner = true;
			this->_sem = this->open_sem(mode);
		}

		inline void
		open(path_type&& name) {
			this->close();
			this->_path = std::forward<path_type>(name),
			this->_owner = false;
			this->_sem = this->open_sem();
		}

		inline void
		wait() {
			UNISTDX_CHECK(::sem_wait(this->_sem));
		}

		inline void
		notify_one() {
			UNISTDX_CHECK(::sem_post(this->_sem));
		}

		inline void
		notify_all() {
			this->notify_all(1000);
		}

		inline void
		notify_all(int n) {
			for (int i=0; i<n; ++i) {
				this->notify_one();
			}
		}

		inline void
		close() {
			if (this->_sem != SEM_FAILED) {
				UNISTDX_CHECK(::sem_close(this->_sem));
				if (this->_owner) {
					UNISTDX_CHECK(::sem_unlink(this->_path.c_str()));
				}
			}
		}

	private:

		inline sem_type
		open_sem(mode_type mode) const {
			sem_type ret;
			UNISTDX_CHECK2(
				ret = ::sem_open(this->_path.c_str(), O_CREAT | O_EXCL, mode, 0),
				SEM_FAILED
			);
			return ret;
		}

		inline sem_type
		open_sem() const {
			sem_type ret;
			UNISTDX_CHECK2(
				ret = ::sem_open(this->_path.c_str(), 0),
				SEM_FAILED
			);
			return ret;
		}

		path_type _path;
		bool _owner = false;
		sem_type _sem = SEM_FAILED;
	};
	#endif

	#if defined(__MACH__)
	typedef std::condition_variable_any thread_semaphore;
	#elif defined(UNISTDX_HAVE_POSIX_SEMAPHORES)
	struct thread_semaphore {

		typedef ::sem_t sem_type;
		typedef std::chrono::system_clock clock_type;
		typedef struct ::timespec timespec_type;

		inline
		thread_semaphore():
		_sem()
		{ this->init_sem(); }

		inline
		~thread_semaphore() {
			this->destroy();
		}

		inline void
		wait() {
			UNISTDX_CHECK(::sem_wait(&_sem));
		}

		template<class Lock>
		void wait(Lock& lock) {
			stdx::unlock_guard<Lock> unlock(lock);
			this->wait();
		}

		template<class Lock, class Pred>
		void wait(Lock& lock, Pred pred) {
			while (!pred()) {
				this->wait(lock);
			}
		}

		template<class Lock, class Rep, class Period>
		std::cv_status
		wait_for(Lock& lock, const std::chrono::duration<Rep,Period>& dur) {
			return this->wait_until(lock, clock_type::now() + dur);
		}

		template<class Lock, class Rep, class Period, class Pred>
		bool
		wait_for(Lock& lock, const std::chrono::duration<Rep,Period>& dur, Pred pred) {
			return this->wait_until(lock, clock_type::now() + dur, pred);
		}

		template<class Lock, class Duration>
		std::cv_status
		wait_until(Lock& lock, const std::chrono::time_point<clock_type,Duration>& tp) {
			using namespace std::chrono;
			const auto s = time_point_cast<seconds>(tp);
			const auto ns = duration_cast<nanoseconds>(tp - s);
			const timespec_type timeout{s.time_since_epoch().count(), ns.count()};
			stdx::unlock_guard<Lock> unlock(lock);
			bits::check_if_not<std::errc::timed_out>(::sem_timedwait(&_sem, &timeout),
				__FILE__, __LINE__, __func__);
			std::cv_status st = std::errc(errno) == std::errc::timed_out
				? std::cv_status::timeout : std::cv_status::no_timeout;
			return st;
		}

		template<class Lock, class Duration, class Pred>
		bool
		wait_until(Lock& lock, const std::chrono::time_point<clock_type,Duration>& tp, Pred pred) {
			while (!pred()) {
				if (this->wait_until(lock, tp) == std::cv_status::timeout) {
					return pred();
				}
			}
			return true;
		}

		template<class Lock, class Clock, class Duration>
		std::cv_status
		wait_until(Lock& lock, const std::chrono::time_point<Clock,Duration>& tp) {
			typedef Clock other_clock;
			const auto delta = tp - other_clock::now();
			const auto new_tp = clock_type::now() + delta;
			return this->wait_until(lock, new_tp);
		}

		template<class Lock, class Clock, class Duration, class Pred>
		bool
		wait_until(Lock& lock, const std::chrono::time_point<Clock,Duration>& tp, Pred pred) {
			typedef Clock other_clock;
			const auto delta = tp - other_clock::now();
			const auto new_tp = clock_type::now() + delta;
			return this->wait_until(lock, new_tp, pred);
		}

		inline void
		notify_one() {
			UNISTDX_CHECK(::sem_post(&_sem));
		}

		inline void
		notify_all() {
			this->notify_all(1000);
		}

		inline void
		notify_all(int n) {
			for (int i=0; i<n; ++i) {
				this->notify_one();
			}
		}

	private:

		inline void
		destroy() {
			UNISTDX_CHECK(::sem_destroy(&_sem));
		}

		inline void
		init_sem() {
			UNISTDX_CHECK(::sem_init(&_sem, 0, 0));
		}

		sem_type _sem;
	};
	#else
	typedef std::condition_variable_any thread_semaphore;
	#endif

}

#endif // SYS_SEMAPHORE_HH
