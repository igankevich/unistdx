#include <gtest/gtest.h>
#include <unistdx/ipc/identity>
#include <unistdx/ipc/process>
#include <unistdx/net/endpoint>
#include <unistdx/net/socket>

TEST(Socket, GetCredentials) {
	const char* path = "\0test_socket";
	sys::endpoint e(path);
	sys::socket sock;
	sock.bind(e);
	sock.setopt(sys::socket::pass_credentials);
	sock.listen();
	sys::process child([&] () {
		sys::socket s(sys::family_type::unix);
		s.setopt(sys::socket::pass_credentials);
		s.connect(e);
		return 0;
	});
	usleep(100000);
	sys::endpoint client_end;
	sys::socket client;
	sock.accept(client, client_end);
	sys::user_credentials creds = client.credentials();
	EXPECT_EQ(child.id(), creds.pid);
	EXPECT_EQ(sys::this_process::user(), creds.uid);
	EXPECT_EQ(sys::this_process::group(), creds.gid);
	sys::proc_status status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}

TEST(Socket, SendFDs) {
	const char* path = "\0testsendfds";
	sys::endpoint e(path);
	sys::socket sock;
	sock.bind(e);
	sock.listen();
	sys::process child([&] () {
		sys::socket s(sys::family_type::unix);
		s.connect(e);
		std::clog << "sending fds: 0 1 2" << std::endl;
		sys::fd_type fds[3] = {0, 1, 2};
		try {
			s.send_fds(fds, 3);
		} catch (const std::exception& err) {
			std::cerr << "Exception: " << err.what() << std::endl;
		}
		return 0;
	});
	usleep(100000);
	sys::endpoint client_end;
	sys::socket client;
	sock.accept(client, client_end);
	sys::fd_type fds[3] = {0, 0, 0};
	client.receive_fds(fds, 3);
	EXPECT_GT(fds[0], 2);
	EXPECT_GT(fds[1], 2);
	EXPECT_GT(fds[2], 2);
	sys::proc_status status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}
