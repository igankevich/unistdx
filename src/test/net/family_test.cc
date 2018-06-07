#include <unistdx/net/family>

#include <unistdx/test/bstream_insert_extract>
#include <unistdx/test/operator>

struct family_test: public ::testing::TestWithParam<sys::family_type> {};

std::vector<sys::family_type> all_families{
	sys::family_type::unspecified,
	sys::family_type::inet,
	sys::family_type::inet6,
	sys::family_type::unix,
	#if defined(UNISTDX_HAVE_NETLINK)
	sys::family_type::netlink
	#endif
};

TEST_P(family_test, bstream_insert_extract) {
	test::bstream_insert_extract(GetParam());
}

INSTANTIATE_TEST_CASE_P(
	for_each_family,
	family_test,
	::testing::ValuesIn(all_families)
);

TEST(family, print) {
	test::stream_insert_equals("inet", sys::family_type::inet);
	test::stream_insert_equals("unknown", sys::family_type(1000));
}

TEST(family, bstream_insert_extract_fails) {
	test::bstream_insert_extract_fails(sys::family_type(1000));
}
