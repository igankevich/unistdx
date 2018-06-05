#include <unistdx/io/epoll_event>

#include <unistdx/test/operator>

TEST(epoll_event, print) {
	test::stream_insert_equals(
		"{fd=1,ev=rwce}",
		sys::epoll_event(
			1,
			sys::event::in | sys::event::out | sys::event::err
		)
	);
	test::stream_insert_equals(
		"{fd=1,ev=--c-}",
		sys::epoll_event(1, sys::event(0))
	);
	sys::epoll_event ev(1, sys::event(0));
	ev.sys::poll_event_base::events = 0;
	test::stream_insert_equals("{fd=1,ev=----}", ev);
}
