#include "Socket.hpp"
#include <gtest/gtest.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080

class SocketTest : public ::testing::Test {
protected:
    int getMaxFileDescriptors() const {
        struct rlimit limit;
        if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
            std::cerr << "Failed to get file descriptor limit." << std::endl;
            return -1;
        }
        if (static_cast<int>(limit.rlim_cur) < 255)
            return static_cast<int>(limit.rlim_cur) - 2;
        // MAXFD = 255 - stdin 1 - stdout 0 = 253;
        return 253;
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


TEST_F(SocketTest, InitializeServers) {
    for (int i = 0; i < MAX_SERVERS; ++i) {
        EXPECT_GE(servers[i]->initialize(), 0) << "Failed to initialize server at index " << i;
    }
}

TEST_F(SocketTest, CheckValidListenSd) {
    for (int i = 0; i < MAX_SERVERS; ++i) {
        servers[i]->initialize();
        EXPECT_GE(servers[i]->getListenSd(), 0) << "Server at index " << i << " has invalid listen_sd_";
    }
}

TEST(SocketInvalidParamsTest, InvalidAddress) {
    server::Socket server("256.256.256.256", SERVER_PORT);
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using an invalid address";
}

TEST(SocketInvalidParamsTest, InvalidPort) {
    server::Socket server(SERVER_ADDR, 65536);
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using an invalid port number";
}

TEST(SocketInvalidParamsTest, NegativePort) {
    server::Socket server(SERVER_ADDR, -1);
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using a negative port number";
}

TEST(SocketInvalidParamsTest, EmptyAddress) {
    server::Socket server("", SERVER_PORT);
    EXPECT_EQ(server.initialize(), -1) << "Expected failure when using an empty address";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
