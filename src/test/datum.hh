#ifndef TEST_DATUM_HH
#define TEST_DATUM_HH

#include <random>
#include <chrono>
#include <unistdx/net/pstream>

typedef std::chrono::nanoseconds::rep Time;

Time time_seed() {
	return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

template<class T>
void rnd(T& val) {
	typedef typename
		std::conditional<std::is_floating_point<T>::value,
			std::uniform_real_distribution<T>,
				std::uniform_int_distribution<T>>::type
					Distribution;
	typedef std::default_random_engine::result_type Res_type;
	static std::default_random_engine generator(static_cast<Res_type>(time_seed()));
	static Distribution distribution(std::numeric_limits<T>::min(),std::numeric_limits<T>::max());
	val = distribution(generator);
}

struct Datum {

	Datum():
		x(0), y(0), z(0),
		u(0), v(0), w(0)
	{
		rnd(x); rnd(y); rnd(static_cast<float&>(z));
		rnd(static_cast<double&>(u)); rnd(v); rnd(w);
	}

	constexpr
	Datum(const Datum& rhs):
		x(rhs.x), y(rhs.y), z(rhs.z),
		u(rhs.u), v(rhs.v), w(rhs.w)
	{}

	bool
	operator==(const Datum& rhs) const {
		return
			x == rhs.x && y == rhs.y && z == rhs.z &&
			u == rhs.u && v == rhs.v && w == rhs.w;
	}

	bool
	operator!=(const Datum& rhs) const {
		return
			x != rhs.x || y != rhs.y || z != rhs.z ||
			u != rhs.u || v != rhs.v || w != rhs.w;
	}

	template <class Ch>
	friend sys::basic_pstream<Ch>&
	operator<<(sys::basic_pstream<Ch>& out, const Datum& rhs) {
		return out
			<< rhs.x << rhs.y << rhs.z
			<< rhs.u << rhs.v << rhs.w;
	}

	template <class Ch>
	friend sys::basic_pstream<Ch>&
	operator>>(sys::basic_pstream<Ch>& in, Datum& rhs) {
		return in
			>> rhs.x >> rhs.y >> rhs.z
			>> rhs.u >> rhs.v >> rhs.w;
	}

	template <class Ch>
	friend std::basic_ostream<Ch>&
	operator<<(std::basic_ostream<Ch>& out, const Datum& rhs) {
		write_raw(out, rhs.x);
		write_raw(out, rhs.y);
		write_raw(out, rhs.z);
		write_raw(out, rhs.u);
		write_raw(out, rhs.v);
		write_raw(out, rhs.w);
		return out;
	}

	template <class Ch>
	friend std::basic_istream<Ch>&
	operator>>(std::basic_istream<Ch>& in, Datum& rhs) {
		read_raw(in, rhs.x);
		read_raw(in, rhs.y);
		read_raw(in, rhs.z);
		read_raw(in, rhs.u);
		read_raw(in, rhs.v);
		read_raw(in, rhs.w);
		return in;
	}

	constexpr static size_t real_size() {
		return
			sizeof(x) + sizeof(y) + sizeof(z) +
			sizeof(u) + sizeof(v) + sizeof(w);
	}

private:

	template<class T, class Ch>
	static void
	write_raw(std::basic_ostream<Ch>& out, const T& rhs) {
		sys::bytes<T> raw = rhs;
		out.write(raw.begin(), raw.size());
	}

	template<class T, class Ch>
	static void
	read_raw(std::basic_istream<Ch>& in, T& rhs) {
		sys::bytes<T> raw;
		in.read(raw.begin(), raw.size());
		rhs = raw;
	}

	int64_t x;
	int32_t y;
	sys::bytes<float> z;
	sys::bytes<double> u;
	int16_t v;
	int8_t w;
	char padding[5] = {};
};

#endif // TEST_DATUM_HH
