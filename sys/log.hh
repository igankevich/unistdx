#ifndef SYS_LOG_HH
#define SYS_LOG_HH

#include <thread>
#include <unordered_map>

#include <syslog.h>

#include <stdx/mutex.hh>

#include <sys/process.hh>

namespace sys {

	template<class Ch, class Tr=std::char_traits<Ch>>
	struct basic_logbuf: public std::basic_streambuf<Ch,Tr> {

		using typename std::basic_streambuf<Ch,Tr>::int_type;
		using typename std::basic_streambuf<Ch,Tr>::traits_type;
		using typename std::basic_streambuf<Ch,Tr>::char_type;
		using typename std::basic_streambuf<Ch,Tr>::pos_type;
		using typename std::basic_streambuf<Ch,Tr>::off_type;
		typedef stdx::spin_mutex mutex_type;
		typedef std::lock_guard<mutex_type> lock_type;
		typedef std::basic_string<Ch,Tr> buf_type;
		typedef std::basic_ostream<Ch,Tr>& stream_type;

		explicit
		basic_logbuf(std::basic_ostream<Ch,Tr>& s):
		_stream(s), _oldbuf(s.rdbuf()), _bufs() {}

		int_type
		overflow(int_type c = traits_type::eof()) override {
			lock_type lock(_mutex);
			this_thread_buf().push_back(c);
			return c;
		}

		std::streamsize
		xsputn(const char_type* s, std::streamsize n) override {
			lock_type lock(_mutex);
			this_thread_buf().append(s, n);
			return n;
		}

		int
		sync() override {
			lock_type lock(_mutex);
			write_log();
			return 0;
		}

		void
		tee(bool rhs) {
			_tee = rhs;
		}

	private:

		void
		write_log() {
			buf_type& buf = this_thread_buf();
			if (!buf.empty()) {
				// write_syslog(buf.c_str());
				if (_tee) {
					_oldbuf->sputc('[');
					const std::string pid = std::to_string(sys::this_process::id());
					_oldbuf->sputn(pid.data(), pid.size());
					_oldbuf->sputc(']');
					_oldbuf->sputc(' ');
					_oldbuf->sputn(buf.data(), buf.size());
					_oldbuf->pubsync();
				}
				buf.clear();
			}
		}

		void
		write_syslog(const char_type* msg) {
			::syslog(LOG_INFO, "%s", msg);
		}

		buf_type&
		this_thread_buf() {
			return _bufs[std::this_thread::get_id()];
		}

		mutex_type _mutex;
		stream_type& _stream;
		std::basic_streambuf<Ch,Tr>* _oldbuf;
		std::unordered_map<std::thread::id, buf_type> _bufs;
		bool _tee = false;
	};

	struct syslog_guard {

		enum options {
			tee=1
		};

		explicit
		syslog_guard(std::ostream& s, options opts=options(0)):
		str(s), newbuf(s), oldbuf(str.rdbuf(&newbuf))
		{
			if (opts & tee) {
				newbuf.tee(true);
			}
		}

		~syslog_guard() {
			str.rdbuf(oldbuf);
		}

	private:
		std::ostream& str;
		basic_logbuf<char> newbuf;
		std::streambuf* oldbuf = nullptr;
	};

}

#endif // SYS_LOG_HH
