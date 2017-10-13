#include <unistdx/io/poller>

#include "semaphore_wait_test.hh"
#include "make_types.hh"

TYPED_TEST_CASE(SemaphoreWaitTest, MAKE_TYPES(sys::event_poller));

TYPED_TEST(SemaphoreWaitTest, EventPollerWaitUntil) {
	this->test_wait_until();
}

TYPED_TEST(SemaphoreWaitTest, EventPollerProducerConsumer) {
	this->test_producer_consumer_thread();
}
