#include <unistdx/ipc/identity>
#include <unistdx/ipc/process>
#include <unistdx/net/endpoint>
#include <unistdx/net/socket>

#include <unistdx/test/operator>

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
	EXPECT_NO_THROW(client.peer_name());
	sys::user_credentials creds = client.credentials();
	EXPECT_EQ(child.id(), creds.pid);
	EXPECT_EQ(sys::this_process::user(), creds.uid);
	EXPECT_EQ(sys::this_process::group(), creds.gid);
	sys::proc_info status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}

TEST(Socket, SendFDs) {
	const char* path = "\0testsendfds";
	sys::endpoint e(path);
	sys::socket sock(e);
	EXPECT_NE("", test::stream_insert(sock));
	sys::process child([&] () {
		sys::socket s(sys::family_type::unix);
		s.connect(e);
		std::clog << "sending fds: 0 1 2" << std::endl;
		sys::fd_type fds[3] = {0, 1, 2};
		int ret = 0;
		try {
			s.send_fds(fds, 3);
		} catch (const std::exception& err) {
			++ret;
			std::cerr << "Exception: " << err.what() << std::endl;
		}
		return ret;
	});
	usleep(100000);
	sys::endpoint client_end;
	sys::socket client;
	sock.accept(client, client_end);
	sys::fd_type fds[3] = {0, 0, 0};
	EXPECT_THROW(client.receive_fds(fds, 10000), std::invalid_argument);
	EXPECT_THROW(client.send_fds(fds, 10000), std::invalid_argument);
	client.receive_fds(fds, 3);
	EXPECT_GT(fds[0], 2);
	EXPECT_GT(fds[1], 2);
	EXPECT_GT(fds[2], 2);
	sys::proc_info status = child.wait();
	EXPECT_EQ(0, status.exit_code());
}

#if defined(UNISTDX_HAVE_TCP_USER_TIMEOUT)
TEST(socket, user_timeout) {
	sys::socket sock;
	sock.bind({{127,0,0,1}, 0});
	EXPECT_NO_THROW(sock.set_user_timeout(std::chrono::seconds(7)));
}
#endif

TEST(socket, bind_connect) {
	EXPECT_NO_THROW(sys::socket({{127,0,0,1}, 0}, {{127,0,0,1},0}));
}
