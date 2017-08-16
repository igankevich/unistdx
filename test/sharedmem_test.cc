#include <stdx/debug.hh>
#include <sys/sharedmem>
//#include <sys/shmembuf.hh>

#include <functional>
#include <gtest/gtest.h>
#include <limits>
#include <random>

template <class T>
struct SharedMemTest: public ::testing::Test {};

typedef ::testing::Types<char, unsigned char> MyTypes;
TYPED_TEST_CASE(SharedMemTest, MyTypes);

TYPED_TEST(SharedMemTest, All) {

	typedef TypeParam T;
	typedef typename sys::shared_mem<T>::size_type size_type;
	const size_type SHMEM_SIZE = 512;
	sys::shared_mem<T> mem1(0666, SHMEM_SIZE);
	sys::shared_mem<T> mem2(mem1.id());
	EXPECT_NE(nullptr, mem1.begin());
	EXPECT_NE(nullptr, mem1.end());
	EXPECT_NE(nullptr, mem2.begin());
	EXPECT_NE(nullptr, mem2.end());
	const size_type real_size = mem1.size();
	EXPECT_EQ(mem1.size(), real_size) << "mem1=" << mem1;
	EXPECT_EQ(mem2.size(), real_size) << "mem2=" << mem2;
	std::default_random_engine rng;
	std::uniform_int_distribution<T> dist(
		std::numeric_limits<T>::min(),
		std::numeric_limits<T>::max()
	);
	std::generate(mem1.begin(), mem1.end(), std::bind(dist, rng));
	EXPECT_EQ(mem1, mem2);
}

/*
   template<class T>
   struct Test_shmembuf: public test::Test<Test_shmembuf<T>> {

    typedef typename sys::shared_mem<T>::size_type size_type;
    typedef sys::shared_mem<T> shmem;
    typedef sys::basic_shmembuf<T> shmembuf;

    void xrun() override {
        shmembuf buf1("/test-shmem-2", 0600);
        shmembuf buf2("/test-shmem-2");
        for (int i=0; i<12; ++i) {
            const size_t size = 2u << i;
            std::clog << "test_shmembuf() middle: size=" << size << std::endl;
            std::vector<T> input(size);
            std::generate(input.begin(), input.end(), test::randomval<T>);
            buf1.lock();
            buf1.sputn(&input.front(), input.size());
            buf1.unlock();
            std::vector<T> output(input.size());
            buf2.lock();
            buf2.sgetn(&output.front(), output.size());
            buf2.unlock();
            test::compare(input, output);
        }
    }

   //	void test_shmembuf_pipe() {
   //		std::string content = "Hello world";
   //		std::stringstream src;
   //		src << content;
   //		shmembuf buf("/test-shmem-3", 0600);
   //		buf.lock();
   //		buf.fill_from(*src.rdbuf());
   //		buf.unlock();
   //		std::stringstream result;
   //		buf.flush_to(*result.rdbuf());
   //		test::equal(result.str(), content);
   //	}

   };
 */
