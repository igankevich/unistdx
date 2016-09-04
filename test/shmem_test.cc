#include <stdx/debug.hh>
#include <sys/sharedmem.hh>
//#include <sys/shmembuf.hh>

#include "test.hh"

template<class T>
struct Test_shmem: public test::Test<Test_shmem<T>> {

	typedef typename sys::shared_mem<T>::size_type size_type;
	typedef sys::shared_mem<T> shmem;

	static bool
	shmem_invariant(shmem& shm) {
		return shm.begin() != nullptr && shm.end() != nullptr;
	}

	void xrun() override {
		const typename sys::shared_mem<T>::size_type SHMEM_SIZE = 512;
		sys::shared_mem<T> mem1(0666, SHMEM_SIZE);
		sys::shared_mem<T> mem2(mem1.id());
		test::invar(shmem_invariant, mem1);
		test::invar(shmem_invariant, mem2);
		const size_type real_size = mem1.size();
		test::equal(mem1.size(), real_size, "bad size: mem1=", std::cref(mem1));
		test::equal(mem2.size(), real_size, "bad size: mem2=", std::cref(mem2));
		std::generate(mem1.begin(), mem1.end(), test::randomval<T>);
		test::compare(mem1, mem2, "mem1 does not match mem2");
	}

};

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

int main() {
	return test::Test_suite{
		new Test_shmem<char>,
		new Test_shmem<unsigned char>
	}.run();
}
