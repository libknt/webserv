#include "Socket.hpp"
#include <gtest/gtest.h>
#include <sys/resource.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080

class SocketTest : public ::testing::Test {
protected:
    int getMaxFileDescriptors() const {
        // MAXFD = 255 - stdin 1 - stdout 0  - 23= 230; testで使用する分を開けておく
        int max_fd = 230;
        struct rlimit limit;
        if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
            std::cerr << "Failed to get file descriptor limit." << std::endl;
            return -1;
        }
        if (static_cast<int>(limit.rlim_cur) < 255)
            return static_cast<int>(limit.rlim_cur) - 20;
        return max_fd;
    }

    const int MAX_SERVERS;
    server::Socket** servers;
    int port = SERVER_PORT;

    SocketTest() : MAX_SERVERS(getMaxFileDescriptors()) {
        servers = new server::Socket*[MAX_SERVERS];
    }

    virtual void SetUp() {
        for (int i = 0; i < MAX_SERVERS; ++i) {
            servers[i] = new server::Socket(SERVER_ADDR, port++);
        }
    }

    virtual void TearDown() {
        for (int i = 0; i < MAX_SERVERS; ++i) {
            delete servers[i];
        }
        delete[] servers;
    }
};

//MAX_SERVERS分のサーバーオブジェクトが正しく初期化できるか
//各サーバーオブジェクトが持っているリッスン用のファイルディスクリプタが有効かどうか
TEST_F(SocketTest, CheckValidListenSd) {
    for (int i = 0; i < MAX_SERVERS; ++i) {
        EXPECT_GE(servers[i]->initialize(), 0) << "Failed to initialize server at index " << i;
        EXPECT_GE(servers[i]->getListenSd(), 0) << "Server at index " << i << " has invalid listen_sd_";
    }
}

//無効なアドレスによる初期化
TEST(SocketInvalidParamsTest, InvalidAddress) {
    server::Socket server("256.256.256.256", 1025);
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using an invalid address";
}

//無効なポートによる初期化
TEST(SocketInvalidParamsTest, InvalidPort) {
    server::Socket server(SERVER_ADDR, 65536);
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using an invalid port number";
}

//ポートが負の値を取らないか
TEST(SocketInvalidParamsTest, NegativePort) {
    server::Socket server(SERVER_ADDR, -1);
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using a negative port number";
}

//空のIPアドレスを指定できるか
TEST(SocketInvalidParamsTest, EmptyAddress) {
    server::Socket server("", 1027);
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using an empty address";
}

//既に使用中のポート番号を指定
TEST(SocketInvalidParamsTest, PortAlreadyInUse) {
    server::Socket server1(SERVER_ADDR, 1028);
    EXPECT_EQ(server1.initialize(), 0) << "Failed to initialize first server.";

    server::Socket server2(SERVER_ADDR, 1028);
    EXPECT_EQ(server2.initialize(), -1) << "Expected failure when using a port that's already in use";
}

//存在しないネットワークインターフェースのアドレス
TEST(SocketInvalidParamsTest, AddressNotAvailable) {
    server::Socket server("192.0.2.0", 1029);  // 使用しないであろうアドレス
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using an address that's not available";
}

//ポート番号0を使用して、サーバーオブジェクトが正しく初期化でき、動的に利用可能なポートが割り当てられるかを確認
TEST(SocketParamsTest, ZeroPort) {
    server::Socket server(SERVER_ADDR, 0);
    EXPECT_EQ(server.initialize(), 0) << "Failed to initialize server with port 0";
    
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    int result = getsockname(server.getListenSd(), (struct sockaddr *)&sin, &len);
    
    ASSERT_NE(result, -1) << "Failed to get socket name.";
    EXPECT_GT(ntohs(sin.sin_port), 0) << "Expected a valid port number after initialization";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
