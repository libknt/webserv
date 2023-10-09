#include "Socket.hpp"
#include <gtest/gtest.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8080

class SocketTest : public ::testing::Test {
protected:
    static const int MAX_SERVERS = 1024;
    server::Socket* servers[MAX_SERVERS];
    int port = SERVER_PORT;

    virtual void SetUp() {
        for (int i = 0; i < MAX_SERVERS; ++i) {
            servers[i] = new server::Socket(SERVER_ADDR, port++);
        }
    }

    virtual void TearDown() {
        for (int i = 0; i < MAX_SERVERS; ++i) {
            delete servers[i];
        }
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
