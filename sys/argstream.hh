#ifndef SYS_ARGSTREAM_HH
#define SYS_ARGSTREAM_HH

#include <streambuf>
#include <vector>
#include <cassert>

namespace sys {

	template<class Ch, class Tr=std::char_traits<Ch>>
	class basic_argbuf: public std::basic_streambuf<Ch,Tr> {

		typedef std::basic_streambuf<Ch,Tr> base_type;

	public:

		using typename base_type::int_type;
		using typename base_type::traits_type;
		using typename base_type::char_type;
		using base_type::pptr;
		using base_type::pbase;
		using base_type::epptr;
		using base_type::pbump;
		typedef std::vector<char_type> arg_type;

		basic_argbuf() {
			_args.reserve(4096);
			_args.emplace_back();
			_rawargs.push_back(nullptr);
		}

		basic_argbuf(basic_argbuf&& rhs):
		_args(std::move(rhs._args)),
		_rawargs(std::move(rhs._rawargs))
		{
			for	(size_t i=0; i<_args.size(); ++i) {
				_rawargs[i] = _args[i].data();
			}
		}

		basic_argbuf(const basic_argbuf&) = delete;

		~basic_argbuf() = default;

		int_type
		overflow(int_type c) override {
			assert(pptr() == epptr());
			if (not traits_type::eq_int_type(c, traits_type::eof())) {
				arg_type& arg = _args.back();
				arg.push_back(traits_type::to_char_type(c));
				if (traits_type::eq_int_type(c, int_type(0))) {
					append_arg();
				}
			}
			return c;
		}

		std::streamsize
		xsputn(const char_type* s, std::streamsize n) override {
			if (n == 1) {
				overflow(traits_type::to_int_type(*s));
			} else {
				arg_type& arg = _args.back();
				const auto old_size = arg.size();
				arg.resize(old_size + n);
				traits_type::copy(arg.data() + old_size, s, n);
			}
			return n;
		}

		char**
		argv() noexcept {
			return _rawargs.data();
		}

		char* const*
		argv() const noexcept {
			return _rawargs.data();
		}

		int
		argc() const noexcept {
			return _rawargs.size()-1;
		}

	private:

		void
		append_arg() {
			_rawargs.back() = _args.back().data();
			_rawargs.push_back(nullptr);
			_args.emplace_back();
		}

		std::vector<arg_type> _args;
		std::vector<char_type*> _rawargs;

	};

	class argstream: public std::ostream {

		typedef char char_type;
		typedef std::char_traits<char_type> traits_type;
		typedef basic_argbuf<char_type,traits_type> argbuf_type;

	public:

		argstream():
		std::ostream(nullptr)
		{ this->init(&_argbuf); }

		argstream(argstream&& rhs):
		std::ostream(),
		_argbuf(std::move(rhs._argbuf))
		{ this->init(&_argbuf); }

		char**
		argv() noexcept {
			return _argbuf.argv();
		}

		char* const*
		argv() const noexcept {
			return _argbuf.argv();
		}

		int
		argc() const noexcept {
			return _argbuf.argc();
		}

		template<class T>
		void
		append(const T& rhs) {
			*this << rhs << '\0';
		}

		template<class T, class ... Args>
		void
		append(const T& first, const Args& ... args) {
			append(first);
			append(args...);
		}

	private:

		argbuf_type _argbuf;

	};

}

#endif // SYS_ARGSTREAM_HH
