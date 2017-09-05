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
	sock.listen();
	sys::process child([&] () {
		sys::socket s;
		s.connect(e);
		return 0;
	});
	usleep(1000);
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
