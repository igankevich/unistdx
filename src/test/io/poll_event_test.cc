#include <unistdx/io/poll_event>

#include <unistdx/test/operator>

TEST(poll_event, print) {
	test::stream_insert_equals("{fd=1,rev=----,ev=--}", sys::poll_event(1));
	test::stream_insert_equals(
		"{fd=1,rev=----,ev=rw}",
		sys::poll_event(1, sys::poll_event::In | sys::poll_event::Out)
	);
	test::stream_insert_equals(
		"{fd=1,rev=rwce,ev=rw}",
		sys::poll_event(
			1,
			sys::poll_event::In | sys::poll_event::Out,
			sys::poll_event::Inout | sys::poll_event::Hup | sys::poll_event::Err
		)
	);
}
