#ifndef SYS_PACKETSTREAM_HH
#define SYS_PACKETSTREAM_HH

#include <cassert>
#include <istream>
#include <ostream>

#include "network_format.hh"
#include <stdx/packetbuf.hh>
#include <unistdx_config>

namespace sys {

	template<class Ch, class Tr=std::char_traits<Ch>, class Size=uint32_t>
	struct basic_packetstream {

		typedef stdx::basic_packetbuf<Ch,Tr> streambuf_type;
		typedef Ch char_type;

		basic_packetstream() = default;
		basic_packetstream(streambuf_type* buf): _buf(buf) {}

		// TODO: delete this
		void flush() { _buf->pubsync(); }
		std::streamsize tellp() { return _buf->pubseekoff(0, std::ios_base::cur, std::ios_base::out); }
		std::streamsize tellg() { return _buf->pubseekoff(0, std::ios_base::cur, std::ios_base::in); }

		streambuf_type*
		rdbuf() {
			return _buf;
		}

		const streambuf_type*
		rdbuf() const {
			return _buf;
		}

		streambuf_type*
		rdbuf(streambuf_type* rhs) {
			streambuf_type* old = _buf;
			_buf = rhs;
			return old;
		}

		void
		sync() {
			_buf->pubsync();
		}

		void
		begin_packet() {
			_buf->begin_packet();
		}

		void
		end_packet() {
			_buf->end_packet();
		}

		bool
		read_packet() {
			return _buf->read_packet();
		}

		void
		append_payload(basic_packetstream& rhs) {
			assert(_buf != nullptr);
			assert(rhs._buf != nullptr);
			stdx::append_payload(*_buf, *rhs._buf);
		}

		explicit
		operator bool() const noexcept {
			return true;
		}

		bool
		operator!() const noexcept {
			return !operator bool();
		}

		basic_packetstream& operator<<(bool rhs) { return write(rhs ? char(1) : char(0)); }
		basic_packetstream& operator<<(char rhs) { return write(rhs); }
		basic_packetstream& operator<<(int8_t rhs)  { return write(rhs); }
		basic_packetstream& operator<<(int16_t rhs) { return write(rhs); }
		basic_packetstream& operator<<(int32_t rhs) { return write(rhs); }
		basic_packetstream& operator<<(int64_t rhs) { return write(rhs); }
		basic_packetstream& operator<<(uint8_t rhs) { return write(rhs); }
		basic_packetstream& operator<<(uint16_t rhs) { return write(rhs); }
		basic_packetstream& operator<<(uint32_t rhs) { return write(rhs); }
		basic_packetstream& operator<<(uint64_t rhs) { return write(rhs); }
		basic_packetstream& operator<<(float rhs) { return write(rhs); }
		basic_packetstream& operator<<(double rhs) { return write(rhs); }
		#if defined(UNISTDX_HAVE_LONG_DOUBLE)
		basic_packetstream& operator<<(long double rhs) { return write(rhs); }
		#endif
		basic_packetstream& operator<<(const std::string& rhs) { return write(rhs); }
		template<class T>
		basic_packetstream& operator<<(const sys::bytes<T>& rhs) {
			return this->write(rhs.begin(), rhs.size());
		}

		basic_packetstream& operator>>(bool& rhs) {
			char c = 0; read(c); rhs = c == 1; return *this;
		}
		basic_packetstream& operator>>(char& rhs) { return read(rhs); }
		basic_packetstream& operator>>(int8_t& rhs) { return read(rhs); }
		basic_packetstream& operator>>(int16_t& rhs) { return read(rhs); }
		basic_packetstream& operator>>(int32_t& rhs) { return read(rhs); }
		basic_packetstream& operator>>(int64_t& rhs) { return read(rhs); }
		basic_packetstream& operator>>(uint8_t& rhs) { return read(rhs); }
		basic_packetstream& operator>>(uint16_t& rhs) { return read(rhs); }
		basic_packetstream& operator>>(uint32_t& rhs) { return read(rhs); }
		basic_packetstream& operator>>(uint64_t& rhs) { return read(rhs); }
		basic_packetstream& operator>>(float& rhs) { return read(rhs); }
		basic_packetstream& operator>>(double& rhs) { return read(rhs); }
		#if defined(UNISTDX_HAVE_LONG_DOUBLE)
		basic_packetstream& operator>>(long double& rhs) { return read(rhs); }
		#endif
		basic_packetstream& operator>>(std::string& rhs) { return read(rhs); }
		template<class T>
		basic_packetstream& operator>>(sys::bytes<T>& rhs) {
			return this->read(rhs.begin(), rhs.size());
		}

		basic_packetstream& write(const Ch* buf, std::streamsize n) {
			_buf->sputn(buf, n);
			return *this;
		}

		basic_packetstream& read(Ch* buf, std::streamsize n) {
			_buf->sgetn(buf, n);
			return *this;
		}

	private:

		template<class T>
		basic_packetstream& write(T rhs) {
		#ifndef IGNORE_ISO_IEC559
			static_assert(
				std::is_integral<T>::value or (
					std::is_floating_point<T>::value and
					std::numeric_limits<T>::is_iec559
				),
				"This system does not support ISO IEC 559"
	            " floating point representation for either float, double or long double"
	            " types, i.e. there is no portable way of"
	            " transmitting floating point numbers over the network"
	            " without precision loss. If all computers in the network do not"
	            " conform to this standard but represent floating point"
	            " numbers exactly in the same way, you can ignore this assertion"
	            " by defining IGNORE_ISO_IEC559."
			);
		#endif
			sys::bytes<T> val = rhs;
			val.to_network_format();
			this->operator<<(val);
			return *this;
		}

		basic_packetstream& write(const std::string& rhs) {
			Size length = static_cast<Size>(rhs.size());
			write(length);
			_buf->sputn(rhs.c_str(), length);
			return *this;
		}

		template<class T>
		basic_packetstream& read(T& rhs) {
			sys::bytes<T> val;
			_buf->sgetn(val.begin(), val.size());
			val.to_host_format();
			rhs = val;
			return *this;
		}

		basic_packetstream& read(std::string& rhs) {
			Size length;
			read(length);
			std::string::value_type* bytes = new std::string::value_type[length];
			_buf->sgetn(bytes, length);
			rhs.assign(bytes, bytes + length);
			delete[] bytes;
			return *this;
		}

		streambuf_type* _buf = nullptr;
	};

	typedef basic_packetstream<char> packetstream;

}

#endif // SYS_PACKETSTREAM_HH
