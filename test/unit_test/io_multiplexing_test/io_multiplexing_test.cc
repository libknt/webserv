#include <cstdlib>
#include <gtest/gtest.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class ServerClientTest : public ::testing::Test {
protected:
	virtual void SetUp() {
		system("./server &");
		sleep(2);
	}

	virtual void TearDown() { system("pkill -f './server'"); }
};

// client connnect test
TEST_F(ServerClientTest, ClientConnectsToServer) {
	int result = system("./client 127.0.0.1 8080 --connect");
	EXPECT_EQ(WEXITSTATUS(result), 0);
}

// echo server test1
TEST_F(ServerClientTest, ClientEchoTest) {
	int result = system("echo \"echo server test\" | ./client 127.0.0.1 8080 --echo-test");
	EXPECT_EQ(WEXITSTATUS(result), 0);
}

// echo server test2
TEST_F(ServerClientTest, ClientEchoTest2) {
	int result = system(
		"echo \"11111111111111112222222232e3323214124142\" | ./client 127.0.0.1 8080 --echo-test");
	EXPECT_EQ(WEXITSTATUS(result), 0);
}

// 1000文字のテスト
TEST_F(ServerClientTest, ClientEchoTest3) {
	int result = system(
		"echo "
		"\"rISrUCfXgS2Seay1TuHddIL/nFIzmYT8rUH611SJq9Tj034jy0R4ZB2IGWFJu/"
		"nH2BBJOyyPR1tZl0pxeZSpgE42krbzftjYMicsVnhel1kIic7gptqjrSZWIujUyWAmpdo8XsOZA5gD2/"
		"M1HN0M8S1bV4wXuu13cytl9pOqkj2sHDQnSG0NREkKOxq477AVPt5CZY0bJ4/"
		"Vra7J0aWybBKnjF8qz0ctvoEouiPJPf7gCaBvMJcs6svOBhf5gRcm3cAKG0AeFx47JeubLIdkkbofp7EnmKHhISWt1"
		"X5JZb0WzT9iVAMoMakWzpk5Ld8gAh7semE1WFzgQwaPcTvTMEgVCm7RfbEj+"
		"AaR3mko8vC2zwfRopdCNKS5HKveK08wGHTIBNCgvcuw97+qCsnq0+"
		"8703ZDwblh2oO3U7lHQoCUtUVDemLqDfNfAY4MHWehYFQf+RLvOfC9XC5eEMNAMMn9li+"
		"qSP49yok1BiyCjuOwM1cRCHamoUyrnH38Vt9d81464UZsEWrOwRHU1rBRjDZ+9NuXj/"
		"8wZgGIakVycNGtwcynDxfTb1OewFkJ5uJiiVZZ7/"
		"9xZy8XMqLgEj7QTVu7w+"
		"VHGtthyLmYH99l4IFnyHc8KaBwLbq7N3SLxldkfJ3Rh9dZ2dhSilbJfMMQu1Pet2IrvQQVEDh77UAeOpprN2+"
		"eqm6GkJx/JK1uFmJXrVzCKSzchRunZ1wu5hwHZwIiiMyECiRpYFnUSro+hIpo/l/"
		"Ftm0xKJTe+"
		"QsVyaVBwVKEuHNbbkwepqAxzsDwlYeTSTHr8Q8NgAohl5I7xEcSjulO73Sstn9BHzgutyWulTutbPrKg15gjG0yI2q"
		"+L1c8hYSCOG69XrAmfCWYzMxH57aasMtJD2ilpH6fRQ0V9RkGjQgtoqxubYhKW2O3tRr1vHZIPvcAU4lU8nXvDwqeg"
		"+wtVkplXoLMALB8WWmWBnXjBllkiTIustLzhzet3lGYoda7Q+b4NWeuIT+Z\" | ./client 127.0.0.1 8080 "
		"--echo-test");
	EXPECT_EQ(WEXITSTATUS(result), 0);
}

// 1023文字のテスト
TEST_F(ServerClientTest, ClientEchoTest4) {
	int result =
		system("echo "
			   "\"p6tl58YRv07m4RvQmUY3bg2XiNzKr0emerRte01OtCnBqW7sXbraIVwYxNvcs+"
			   "7I1nt9IfOBGrHwc32jN9wBIJmAxf7oPdoWPSXWU4ftxA8d5ZB/k9LVT455PJZnSX8Bb6ub7tnNy/"
			   "OpTwCJdl4mTLNFDH6G+Uk8J+NbPxeH/"
			   "rzHxWLDmj2ORhEtblKrLof0xW4k0YBBm7YDWdH6Fqaq9tuRfiQTQtSLJCG3IN3D3ZvvYZoJImcNyVxIMPcq"
			   "tw1aUabS5G+v7Z6RSiaDDFQQRhhKbB/YVXEmaUmGb8bYf2/XcgXW0zrjF/mdNBIbNap3ppMr/"
			   "NqRjbbwf8BqSoB1DFKtOIsy91tR/"
			   "74cG62MXvD6sVXwjzWenQqyeL0gj2nEFzJLkbEBUJMuDD1DNxQi8ycZ5zY1wrwhWbYEqNXUpdtlzbVxZGtV"
			   "13rqRxyau4psA5Aa8tQ5krnt/"
			   "tVRR+2phuAeUa+kL4VK6WtPIRI7cpY7+"
			   "B5Qu50tbnUX5qBbcQTHY1sR89NszmMHyQKSVzvsGISeoLs95Mp0qItMNkkQo75/luxcBLrX5ElHRd/"
			   "jKGu+d2KipRCnOyv0qYyHQi03AJoqPhYjWK773+QqCjMBdLGvwK98Sm8YEQi2mX+"
			   "NWHWlpJ8kgDEC0HEZ8aatmwkHrDHxuiLU8ifWEIXaz/+hrYo7G3yu2lZnCv6XUFYqE/9OVgA/"
			   "56xKFYo4buFvDkJjIogSMGjBJ8AjgNxKRAVpABSXY1WJ+"
			   "kQYJXozEnngv2SLjgrAEcaxwfMxkajZVHKUOYWTweSaAYDcyZpKR0XM6uqqf+"
			   "NtdMn1FUVj2UdZUxJ25rki0fGfmuMD2sk4/we8uGNuMGOVdx/"
			   "G6q0hXsBf6sdXE6VoQ7YLvJ5Il91fNF6TISNDggDniUftYoPMyKfryPf1HJahbjkuRNWJH6Sd6SQpgipc8W"
			   "v4/8dJ9bR//6/xYp3JkftkVrE9iJg9UwWzXlzlO4zEqn60eZ1xaK6OUvSbeGjDD/IQHiJgmPVfO2n\" | "
			   "./client 127.0.0.1 8080 --echo-test");
	EXPECT_EQ(WEXITSTATUS(result), 0);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
