#ifndef SYS_BITS_BASIC_OSTREAM_ITERATOR_HH
#define SYS_BITS_BASIC_OSTREAM_ITERATOR_HH

namespace sys {

	template<class Stream, class Value>
	class basic_ostream_iterator:
		public std::iterator<std::output_iterator_tag, Value>
	{

		typedef Stream stream_type;
		typedef typename std::iterator<std::output_iterator_tag, Value> base_type;

	public:

		using base_type::iterator_category;

		typedef Value value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;

		explicit
		basic_ostream_iterator(stream_type& rhs):
		_stream(&rhs)
		{}

		basic_ostream_iterator() = default;

		inline
		~basic_ostream_iterator() = default;

		basic_ostream_iterator(const basic_ostream_iterator&) = default;

		inline basic_ostream_iterator&
		operator=(const basic_ostream_iterator&) = default;

		bool
		operator==(const basic_ostream_iterator& rhs) const noexcept {
			return this->_stream == rhs._stream;
		}

		bool
		operator!=(const basic_ostream_iterator& rhs) const noexcept {
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

		inline basic_ostream_iterator&
		operator++() {
			this->write(); return *this;
		}

		inline basic_ostream_iterator
		operator++(int) {
			basic_ostream_iterator tmp(*this); this->write(); return tmp;
		}

	private:

		inline void
		write() {
 			if (not (*_stream << _value)) {
				_stream = nullptr;
			}
		}

		stream_type* _stream = nullptr;
		value_type _value;

	};

}

#endif // SYS_BITS_BASIC_OSTREAM_ITERATOR_HH

