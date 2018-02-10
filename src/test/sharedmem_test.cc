#include <unistdx/base/make_object>
#include <unistdx/ipc/sharedmem>
#include <unistdx/ipc/shmembuf>
#include <unistdx/ipc/process>
#include <unistdx/util/system>

#include <functional>
#include <gtest/gtest.h>
#include <limits>
#include <random>
#include <mutex>

#include "make_types.hh"

template <class T>
struct SharedMemTest: public ::testing::Test {};

TYPED_TEST_CASE(SharedMemTest, MAKE_TYPES(char, unsigned char));

TYPED_TEST(SharedMemTest, SharedMem) {
	typedef TypeParam T;
	typedef typename sys::shared_mem<T>::size_type size_type;
	const size_type SHMEM_SIZE = 4096;
	sys::shared_mem<T> mem1(0666, SHMEM_SIZE);
	sys::shared_mem<T> mem2(mem1.id());
	// check some invariants
	EXPECT_GE(mem1.size(), SHMEM_SIZE);
	EXPECT_GE(mem2.size(), SHMEM_SIZE);
	EXPECT_EQ(mem1.size(), mem2.size());
	EXPECT_TRUE(mem1.owner());
	EXPECT_FALSE(mem2.owner());
	EXPECT_TRUE(static_cast<bool>(mem1))
		<< "ptr=" << ((void*)mem1.ptr())
		<< ",id=" << mem1.id();
	EXPECT_TRUE(static_cast<bool>(mem2))
		<< "ptr=" << ((void*)mem2.ptr())
		<< ",id=" << mem2.id();
	EXPECT_FALSE(!mem1);
	EXPECT_FALSE(!mem2);
	EXPECT_NE(nullptr, mem1.begin());
	EXPECT_NE(nullptr, mem1.end());
	EXPECT_NE(nullptr, mem2.begin());
	EXPECT_NE(nullptr, mem2.end());
	EXPECT_EQ(mem1.size(), std::distance(mem1.begin(), mem1.end()));
	EXPECT_EQ(mem2.size(), std::distance(mem2.begin(), mem2.end()));
	std::default_random_engine rng;
	std::uniform_int_distribution<T> dist('a', 'z');
	std::generate(mem1.begin(), mem1.end(), std::bind(dist, rng));
	EXPECT_EQ(mem1, mem2);
	// close multiple times
	EXPECT_NO_THROW(mem1.close());
	EXPECT_NO_THROW(mem2.close());
	EXPECT_NO_THROW(mem1.close());
	EXPECT_NO_THROW(mem2.close());
	EXPECT_FALSE(static_cast<bool>(mem1));
	EXPECT_FALSE(static_cast<bool>(mem2));
}

TYPED_TEST(SharedMemTest, SharedMemBuf) {
	typedef TypeParam T;
	typedef typename sys::shared_mem<T> shmem;
	typedef typename sys::basic_shmembuf<T> shmembuf;
	typedef typename shmem::size_type size_type;
	typedef std::lock_guard<shmembuf> shmembuf_guard;
	const size_type SHMEM_SIZE = sys::page_size()*4;
	shmembuf buf1(shmem(0600, SHMEM_SIZE));
	// generated random data
	const size_t ninputs = 12;
	std::default_random_engine rng;
	std::uniform_int_distribution<T> dist('a', 'z');
	std::vector<std::vector<T>> inputs;
	for (size_t i=0; i<ninputs; ++i) {
		const size_t size = 2u << i;
		inputs.emplace_back(size);
		std::generate(
			inputs.back().begin(),
			inputs.back().end(),
			std::bind(dist, rng)
		);
	}
	sys::process consumer([&] () {
		shmembuf buf2(shmem(buf1.mem().id()));
		std::clog << "buf2.mem()=" << buf2.mem() << std::endl;
		bool success = true;
		for (size_t i=0; i<ninputs; ++i) {
			const std::vector<T>& input = inputs[i];
			std::vector<T> output(input.size());
			{
				shmembuf_guard lock(buf2);
				buf2.sgetn(output.data(), output.size());
			}
			EXPECT_EQ(input, output);
			if (input != output) {
				success = false;
			}
		}
		return success ? EXIT_SUCCESS : EXIT_FAILURE;
	});
	for (size_t i=0; i<ninputs; ++i) {
		//sleep(1);
		const std::vector<T>& input = inputs[i];
		shmembuf_guard lock(buf1);
		buf1.sputn(input.data(), input.size());
	}
	sys::proc_status status = consumer.wait();
	EXPECT_TRUE(status.exited() && status.exit_code() == EXIT_SUCCESS);
}
