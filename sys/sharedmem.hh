#ifndef SYS_SHAREDMEM_HH
#define SYS_SHAREDMEM_HH

#include <memory>
#include <vector>
#include <utility>
#include <cstring>
#include <chrono>

#include <unistd.h>
#include <bits/sharedmem>

#include <sys/fildes.hh>
#include <sys/process.hh>

namespace sys {

	typedef ::shmid_ds basic_shmstat_type;
	typedef int shm_type;

	class sharedmem_stat: public ::shmid_ds {

		typedef std::chrono::system_clock clock_type;
		typedef clock_type::time_point tp_type;
		typedef ::shmatt_t num_attaches_type;

	public:

		typedef size_t size_type;

		explicit
		sharedmem_stat(shm_type id) {
			getstat(id);
		}

		size_type
		segment_size() const noexcept {
			return shm_segsz;
		}

		pid_type
		creator() const noexcept {
			return shm_cpid;
		}

		pid_type
		last_user() const noexcept {
			return shm_lpid;
		}

		tp_type
		last_attach() const noexcept {
			return clock_type::from_time_t(shm_atime);
		}

		tp_type
		last_detach() const noexcept {
			return clock_type::from_time_t(shm_dtime);
		}

		tp_type
		last_change() const noexcept {
			return clock_type::from_time_t(shm_ctime);
		}

		num_attaches_type
		num_attaches() const noexcept {
			return shm_nattch;
		}

	private:

		void
		getstat(shm_type id) {
			UNISTDX_CHECK(::shmctl(id, IPC_STAT, this));
		}

	};

	namespace ipc {

		enum ipc_mode_type {
			create = IPC_CREAT,
			exclusive = IPC_EXCL,
			no_wait = IPC_NOWAIT
		};

	}

	namespace shm {

		enum shm_flags_type {
			huge_pages = UNISTDX_SHM_HUGETLB,
			huge_2mb = UNISTDX_SHM_HUGE_2MB,
			huge_1gb = UNISTDX_SHM_HUGE_1GB,
			no_reserve = UNISTDX_SHM_NORESERVE
		};

	}

	template<class T>
	struct shared_mem {

		typedef ::key_t key_type;
		typedef size_t size_type;
		typedef void* addr_type;
		typedef T value_type;
		typedef T* iterator;
		typedef const T* const_iterator;

		shared_mem(mode_type mode, size_type guaranteed_size) {
			open_as_owner(mode|ipc::create, guaranteed_size);
		}

		explicit
		shared_mem(shm_type shm) {
			open_as_user(shm);
		}

		shared_mem(shared_mem&& rhs):
		_shm(rhs._shm),
		_addr(rhs._addr),
		_owner(rhs._owner),
		_size(rhs._size)
		{
			rhs._addr = nullptr;
			rhs._owner = false;
		}

		shared_mem() = default;
		shared_mem(const shared_mem&) = delete;

		~shared_mem() {
			this->close();
		}

		shared_mem&
		operator=(const shared_mem&) = delete;

		shared_mem&
		operator=(shared_mem&& rhs) noexcept {
			std::swap(_shm, rhs._shm);
			std::swap(_addr, rhs._addr);
			std::swap(_size, rhs._size);
			std::swap(_owner, rhs._owner);
			return *this;
		}

		addr_type
		ptr() noexcept {
			return _addr;
		}

		const addr_type
		ptr() const noexcept {
			return _addr;
		}

		size_type
		size() const noexcept {
			return _size;
		}

		size_type
		size_in_bytes() const noexcept {
			return size()*sizeof(value_type);
		}

		bool
		is_owner() const noexcept {
			return _owner;
		}

		iterator
		begin() noexcept {
			return static_cast<iterator>(_addr);
		}

		iterator
		end() noexcept {
			return static_cast<iterator>(_addr) + _size;
		}

		const_iterator
		begin() const noexcept {
			return static_cast<iterator>(_addr);
		}

		const_iterator
		end() const noexcept {
			return static_cast<iterator>(_addr) + _size;
		}

		shm_type
		id() const noexcept {
			return _shm;
		}

		explicit
		operator bool() const noexcept {
			return _addr and _shm;
		}

		bool
		operator!() const noexcept {
			return !operator bool();
		}

		void
		open_as_user(shm_type id) {
			_shm = id;
			_addr = attach(id);
			_size = getsize();
		}

		void
		open_as_owner(mode_type mode, size_type size) {
			_shm = open(IPC_PRIVATE, size, mode);
			_addr = attach(_shm);
			_size = getsize();
			_owner = true;
			memzero();
		}

		void
		close() {
			detach();
			remove();
		}

	private:

		void
		remove() {
			if (is_owner()) {
				UNISTDX_CHECK(::shmctl(_shm, IPC_RMID, 0));
				this->_shm = 0;
			}
		}

		void
		memzero() noexcept {
			std::memset(ptr(), 0, size_in_bytes());
		}

		static shm_type
		open(key_type key, size_type size, int shmflags) {
			int ret;
			UNISTDX_CHECK(ret = ::shmget(key, size, shmflags));
			return ret;
		}

		static addr_type
		attach(shm_type s) {
			addr_type ret;
			UNISTDX_CHECK2(ret = ::shmat(s, nullptr, 0), nullptr);
			return ret;
		}

		void
		detach() {
			if (this->_addr) {
				UNISTDX_CHECK(::shmdt(_addr));
				this->_addr = nullptr;
			}
		}

		size_type
		getsize() const {
			sharedmem_stat stat(_shm);
			return stat.segment_size() / sizeof(value_type);
		}

		friend std::ostream&
		operator<<(std::ostream& out, const shared_mem& rhs) {
			return out << stdx::make_object(
				"addr", rhs.ptr(),
				"size", rhs.size(),
				"owner", rhs.is_owner(),
				"shm", rhs.id()
			);
		}

		shm_type _shm = 0;
		addr_type _addr = nullptr;
		size_type _size = 0;
		bool _owner = false;

	};

}

#endif // SYS_SHAREDMEM_HH
