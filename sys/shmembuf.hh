#ifndef SYS_SHMEMBUF_HH
#define SYS_SHMEMBUF_HH

#include <factory/error.hh>
#include <stdx/packetbuf.hh>
#include <stdx/mutex.hh>
#include <sys/sharedmem.hh>

namespace sys {

	template<class Ch, class Tr=std::char_traits<Ch>>
	struct basic_shmembuf: public stdx::basic_packetbuf<Ch,Tr> {

		using typename stdx::basic_packetbuf<Ch,Tr>::int_type;
		using typename stdx::basic_packetbuf<Ch,Tr>::traits_type;
		using typename stdx::basic_packetbuf<Ch,Tr>::char_type;
		using typename stdx::basic_packetbuf<Ch,Tr>::pos_type;
		using typename stdx::basic_packetbuf<Ch,Tr>::off_type;

		typedef typename sys::shared_mem<char_type>::size_type size_type;
		typedef stdx::spin_mutex mutex_type;

		explicit
		basic_shmembuf(path_type&& path, sys::mode_type mode):
		_sharedmem(std::forward<path_type>(path), 512, mode, BUFFER_PROJID),
		_sharedpart(new (_sharedmem.ptr()) shmem_header)
		{
			char_type* ptr = _sharedmem.begin() + sizeof(shmem_header);
			this->setg(ptr, ptr, ptr);
			this->setp(ptr, _sharedmem.end());
			#ifndef NDEBUG
			this->debug("basic_shmembuf()");
			#endif
		}

		explicit
		basic_shmembuf(path_type&& path):
		_sharedmem(std::forward<path_type>(path), BUFFER_PROJID),
		_sharedpart(static_cast<shmem_header*>(_sharedmem.ptr()))
		{
			char_type* ptr = _sharedmem.begin() + sizeof(shmem_header);
			this->setg(ptr, ptr, ptr);
			this->setp(ptr, _sharedmem.end());
			this->sync_sharedmem();
			#ifndef NDEBUG
			this->debug("basic_shmembuf(int)");
			#endif
		}

		basic_shmembuf(basic_shmembuf&& rhs):
		_sharedmem(std::move(rhs._sharedmem)),
		_sharedpart(rhs._sharedpart)
		{}

		~basic_shmembuf() = default;

		int_type
		overflow(int_type c = traits_type::eof()) override {
			int_type ret;
			if (c != traits_type::eof()) {
				this->grow_sharedmem();
				*this->pptr() = c;
				this->pbump(1);
				this->setg(this->eback(), this->gptr(), this->egptr()+1);
				ret = traits_type::to_int_type(c);
			} else {
				ret = traits_type::eof();
			}
			return ret;
		}

		int_type
		underflow() override {
			int_type ret;
			if (this->egptr() < this->pptr()) {
				this->setg(this->eback(), this->gptr(), this->pptr());
				ret = traits_type::to_int_type(*this->gptr());
			} else {
				ret = traits_type::eof();
			}
			return ret;
		}

		std::streamsize
		xsputn(const char_type* s, std::streamsize n) override {
			#ifndef NDEBUG
			this->debug("xsputn");
			#endif
			char_type* first = const_cast<char_type*>(s);
			char_type* last = first + n;
			while (first != last) {
				if (this->epptr() == this->pptr()) {
					this->overflow(*first);
					++first;
				}
				const std::streamsize
				m = std::min(last-first, this->epptr() - this->pptr());
				traits_type::copy(this->pptr(), first, m);
				this->pbump(m);
				first += m;
			}
			return n;
		}

		void lock() {
			#ifndef NDEBUG
			this->debug("locking");
			#endif
			this->sync_sharedmem();
			this->mutex().lock();
			#ifndef NDEBUG
			this->debug("locked");
			#endif
		}

		void unlock() {
			this->writeoffs();
			this->mutex().unlock();
			#ifndef NDEBUG
			this->debug("unlock");
			#endif
		}

	private:

		mutex_type& mutex() { return _sharedpart->mtx; }

		#ifndef NDEBUG
		void debug(const char* tag) {
			stdx::debug_message("shmembuf", tag)
				<< "goff=" << _sharedpart->goff
				<< ",poff=" << _sharedpart->poff
				<< ",gptr=" << static_cast<std::ptrdiff_t>(this->gptr() - this->eback())
				<< ",pptr=" << static_cast<std::ptrdiff_t>(this->pptr() - this->pbase())
				<< ",shmem=" << _sharedmem;
		}
		#endif

		void
		grow_sharedmem() {
			#ifndef NDEBUG
			debug("grow");
			#endif
			_sharedmem.resize(_sharedmem.size() * size_type(2));
			_sharedpart = static_cast<shmem_header*>(_sharedmem.ptr());
			this->updatebufs();
			#ifndef NDEBUG
			debug("after grow");
			#endif
		}

		void
		sync_sharedmem() {
			_sharedmem.sync();
			_sharedpart = static_cast<shmem_header*>(_sharedmem.ptr());
			this->readoffs();
		}

		void
		updatebufs() {
			const std::ptrdiff_t poff = this->pptr() - this->pbase();
			const std::ptrdiff_t goff = this->gptr() - this->eback();
			char_type* base = _sharedmem.begin() + sizeof(shmem_header);
			char_type* end = _sharedmem.end();
			this->setp(base, end);
			this->pbump(poff);
			this->setg(base, base + goff, base + poff);
		}

		void
		writeoffs() {
			_sharedpart->goff = static_cast<pos_type>(this->gptr() - this->eback());
			_sharedpart->poff = static_cast<pos_type>(this->pptr() - this->pbase());
			#ifndef NDEBUG
			this->debug("writeoffs");
			#endif
		}

		void
		readoffs() {
			const pos_type goff = _sharedpart->goff;
			const pos_type poff = _sharedpart->poff;
			char_type* base = _sharedmem.begin() + sizeof(shmem_header);
			char_type* end = _sharedmem.end();
			this->setp(base, end);
			this->pbump(poff);
			this->setg(base, base + goff, base + poff);
			#ifndef NDEBUG
			this->debug("readoffs");
			#endif
		}

		sys::shared_mem<char_type> _sharedmem;
		struct shmem_header {
			mutex_type mtx;
			pos_type goff = 0;
			pos_type poff = 0;
		} *_sharedpart = nullptr;

		constexpr static const char BUFFER_PROJID = 'b';
	};

	typedef basic_shmembuf<char> shmembuf;

}

#endif // SYS_SHMEMBUF_HH
