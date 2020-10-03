#include <unistdx/ipc/semaphore>
#include <unistdx/test/make_types>
#include <unistdx/test/semaphore_base>
#include <unistdx/test/semaphore_wait_test>

struct SemaphoreTest: public ::testing::Test {};

#if defined(UNISTDX_HAVE_SYS_SEM_H)
TEST(SemaphoreTest, sysv_semaphore) {
    test_semaphore<sys::sysv_semaphore>();
}

TYPED_TEST_CASE(SemaphoreWaitTest, MAKE_TYPES(sys::sysv_semaphore));

TYPED_TEST(SemaphoreWaitTest, WaitUntil) {
    this->test_wait_until();
}

TYPED_TEST(SemaphoreWaitTest, ProducerConsumer) {
    this->test_producer_consumer_thread();
}

template <class T>
using SemaphoreProcessTest = SemaphoreWaitTest<T>;

TYPED_TEST_CASE(SemaphoreProcessTest, MAKE_TYPES(sys::sysv_semaphore));

TYPED_TEST(SemaphoreProcessTest, ProducerConsumer) {
    this->test_producer_consumer_process();
}
#endif

int main(int argc, char* argv[]) {
    test_semaphore_is_available<sys::sysv_semaphore>();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
