#ifndef SYS_BITS_BASIC_ISTREAM_ITERATOR_HH
#define SYS_BITS_BASIC_ISTREAM_ITERATOR_HH

namespace sys {

	template<class Stream, class Value>
	class basic_istream_iterator: public std::iterator<std::input_iterator_tag, Value> {

		typedef Stream stream_type;

	public:

		using typename std::iterator<std::input_iterator_tag, Value>::iterator_category;

		typedef Value value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;

		explicit
		basic_istream_iterator(stream_type& rhs):
		_stream(&rhs)
		{ next(); }

		basic_istream_iterator() = default;

		inline
		~basic_istream_iterator() = default;

		basic_istream_iterator(const basic_istream_iterator&) = default;

		basic_istream_iterator(basic_istream_iterator&& rhs):
		_stream(rhs._stream),
		_value(std::move(rhs._value))
		{ rhs._stream = nullptr; }

		inline basic_istream_iterator&
		operator=(const basic_istream_iterator&) = default;

		bool
		operator==(const basic_istream_iterator& rhs) const noexcept {
			return this->_stream == rhs._stream;
		}

		bool
		operator!=(const basic_istream_iterator& rhs) const noexcept {
			return !operator==(rhs);
		}

		inline reference
		operator*() noexcept {
			return this->_value;
		}

		const_reference
		operator*() const noexcept {
			return this->_value;
		}

		inline pointer
		operator->() noexcept {
			return &this->_value;
		}

		const_pointer
		operator->() const noexcept {
			return &this->_value;
		}

		inline basic_istream_iterator&
		operator++() {
			this->next(); return *this;
		}

		inline basic_istream_iterator
		operator++(int) {
			basic_istream_iterator tmp(*this); this->next(); return tmp;
		}

	private:

		inline void
		next() {
 			if (not (*_stream >> _value)) {
				_stream = nullptr;
			}
		}

		stream_type* _stream = nullptr;
		value_type _value;

	};

}

#endif // SYS_BITS_BASIC_ISTREAM_ITERATOR_HH
