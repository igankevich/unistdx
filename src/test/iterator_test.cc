#include <algorithm>
#include <functional>
#include <gtest/gtest.h>
#include <queue>
#include <random>
#include <unistdx/bits/paired_iterator>
#include <unistdx/it/queue_popper>
#include <unistdx/it/queue_pusher>
#include <vector>

TEST(PairedIteratorTest, Copy) {
	typedef uint32_t T;
	std::mt19937 rng;
	std::vector<T> x(20), y(20);
	std::vector<T> z(20), w(20);
	std::generate(
		x.begin(),
		x.end(),
		[&rng] () { return rng(); }
	);
	auto beg1 = sys::bits::make_paired(x.begin(), y.begin());
	auto end1 = sys::bits::make_paired(x.end(), y.end());
	auto beg2 = sys::bits::make_paired(z.begin(), w.begin());
	std::copy(beg1, end1, beg2);
	EXPECT_EQ(x, z);
	EXPECT_EQ(y, w);
}

#define MAKE_QUEUE_PUSHER_TEST(Queue, Pusher) \
	std::default_random_engine rng; \
	Queue queue; \
	std::generate_n(Pusher(queue), 100, std::ref(rng)); \
	EXPECT_EQ(100, queue.size());

#define MAKE_QUEUE_POPPER_TEST(Queue, Popper) \
	std::vector<T> result; \
	std::copy( \
		Popper(queue), \
		Popper##_end(queue), \
		std::back_inserter(result) \
	); \
	EXPECT_EQ(100, result.size()); \
	EXPECT_EQ(0, queue.size());

typedef std::default_random_engine::result_type T;

TEST(QueuePusherAndPopper, Queue) {
	MAKE_QUEUE_PUSHER_TEST(std::queue<T>, sys::queue_pusher);
	MAKE_QUEUE_POPPER_TEST(std::queue<T>, sys::queue_popper);
}

TEST(QueuePusherAndPopper, PriorityQueue) {
	MAKE_QUEUE_PUSHER_TEST(std::priority_queue<T>, sys::priority_queue_pusher);
	MAKE_QUEUE_POPPER_TEST(std::priority_queue<T>, sys::priority_queue_popper);
}

TEST(QueuePusherAndPopper, Deque) {
	MAKE_QUEUE_PUSHER_TEST(std::deque<T>, sys::deque_pusher);
	MAKE_QUEUE_POPPER_TEST(std::deque<T>, sys::deque_popper);
}
