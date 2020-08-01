/*
UNISTDX — C++ library for Linux system calls.
© 2020 Ivan Gankevich

This file is part of UNISTDX.

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <unistdx/ipc/identity>
#include <unistdx/ipc/process>
#include <unistdx/net/socket>
#include <unistdx/net/socket_address>

#include <unistdx/test/operator>

TEST(Socket, GetCredentials) {
    const char* path = "\0test_socket";
    sys::socket_address e(path);
    sys::socket sock;
    sock.bind(e);
    sock.set(sys::socket::options::pass_credentials);
    sock.listen();
    sys::process child([&] () {
        sys::socket s(sys::family_type::unix);
        s.set(sys::socket::options::pass_credentials);
        s.connect(e);
        return 0;
    });
    usleep(100000);
    sys::socket_address client_end;
    sys::socket client;
    sock.accept(client, client_end);
    EXPECT_NO_THROW(client.peer_name());
    sys::user_credentials creds = client.credentials();
    EXPECT_EQ(child.id(), creds.pid);
    EXPECT_EQ(sys::this_process::user(), creds.uid);
    EXPECT_EQ(sys::this_process::group(), creds.gid);
    sys::process_status status = child.wait();
    EXPECT_EQ(0, status.exit_code());
}

TEST(Socket, SendFDs) {
    const char* path = "\0testsendfds";
    sys::socket_address e(path);
    sys::socket sock(e);
    sock.unsetf(sys::open_flag::non_blocking);
    EXPECT_NE("", test::stream_insert(sock));
    sys::process child([&] () {
        sys::socket s(sys::family_type::unix);
        s.unsetf(sys::open_flag::non_blocking);
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
    sys::socket_address client_end;
    sys::socket client;
    sock.accept(client, client_end);
    client.unsetf(sys::open_flag::non_blocking);
    sys::fd_type fds[3] = {0, 0, 0};
    EXPECT_THROW(client.receive_fds(fds, 10000), std::invalid_argument);
    EXPECT_THROW(client.send_fds(fds, 10000), std::invalid_argument);
    client.receive_fds(fds, 3);
    EXPECT_GT(fds[0], 2);
    EXPECT_GT(fds[1], 2);
    EXPECT_GT(fds[2], 2);
    sys::process_status status = child.wait();
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
    try {
        sys::socket sock({{127,0,0,1}, 0}, {{127,0,0,1},0});
    } catch (const sys::bad_call& err) {
        EXPECT_EQ(std::errc::operation_in_progress, err.errc());
    }
}
