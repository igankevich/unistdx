#include <algorithm>
#include <functional>
#include <gtest/gtest.h>
#include <queue>
#include <random>
#include <unistdx/it/queue_popper>
#include <unistdx/it/queue_pusher>
#include <vector>

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
