#include <unistdx/io/two_way_pipe>

#include <unistdx/test/operator>

TEST(two_way_pipe, print) {
	sys::two_way_pipe p;
	EXPECT_NE("", test::stream_insert(p));
}

TEST(two_way_pipe, close_in_parent) {
	sys::two_way_pipe p;
	EXPECT_NO_THROW(p.validate());
	p.close_in_parent();
	EXPECT_NO_THROW(p.validate());
}

TEST(two_way_pipe, close_unused) {
	sys::two_way_pipe p;
	EXPECT_NO_THROW(p.validate());
	p.close_unused();
	EXPECT_NO_THROW(p.validate());
}

TEST(two_way_pipe, close) {
	sys::two_way_pipe p;
	EXPECT_NO_THROW(p.validate());
	p.close();
	EXPECT_THROW(p.validate(), sys::bad_call);
}

TEST(two_way_pipe, close_in_child) {
	sys::two_way_pipe p;
	EXPECT_NO_THROW(p.validate());
	p.close_in_child();
	EXPECT_THROW(p.validate(), sys::bad_call);
}

TEST(two_way_pipe, child_close_in_child) {
	sys::two_way_pipe p;
	EXPECT_NO_THROW(p.validate());
	sys::process child{
		[&p] () {
			int ret = 0;
			try {
				p.close_in_child();
			} catch (...) {
				++ret;
			}
			try {
				p.validate();
			} catch (...) {
				++ret;
			}
			try {
				p.remap_in_child();
			} catch (...) {
				++ret;
			}
			try {
				p.remap_in_child(1000, 1001);
			} catch (...) {
				++ret;
			}
			return ret;
		}
	};
	p.close_in_parent();
	EXPECT_NO_THROW(p.validate());
	sys::proc_info status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}

TEST(two_way_pipe, child_close_unused) {
	sys::two_way_pipe p;
	EXPECT_NO_THROW(p.validate());
	sys::process child{
		[&p] () {
			int ret = 0;
			try {
				p.close_unused();
			} catch (...) {
				++ret;
			}
			try {
				p.validate();
			} catch (...) {
				++ret;
			}
			return ret;
		}
	};
	p.close_in_parent();
	EXPECT_NO_THROW(p.validate());
	sys::proc_info status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}

TEST(two_way_pipe, open) {
	sys::two_way_pipe p;
	EXPECT_NO_THROW(p.validate());
	p.open();
	EXPECT_NO_THROW(p.validate());
}
