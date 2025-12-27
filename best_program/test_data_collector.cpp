#include <gtest/gtest.h>
#include <thread>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <endian.h>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>

std::string timestamp_to_string(int64_t timestamp_us) {
    time_t seconds = timestamp_us / 1000000;
    struct tm* timeinfo = localtime(&seconds);
    std::ostringstream oss;
    oss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

uint8_t calculate_checksum(const uint8_t* data, size_t length) {
    uint8_t s = 0;
    for (size_t i = 0; i < length; i++) s += data[i];
    return s;
}

uint64_t be64toh_custom(const uint8_t* data) {
    uint64_t value;
    memcpy(&value, data, 8);
    return be64toh(value);
}

int32_t be32toh_signed(const uint8_t* data) {
    int32_t value;
    memcpy(&value, data, 4);
    return (int32_t)be32toh((uint32_t)value);
}

int16_t be16toh_signed(const uint8_t* data) {
    int16_t value;
    memcpy(&value, data, 2);
    return (int16_t)be16toh((uint16_t)value);
}

float be32toh_float(const uint8_t* data) {
    uint32_t temp;
    memcpy(&temp, data, 4);
    temp = be32toh(temp);
    float result;
    memcpy(&result, &temp, 4);
    return result;
}

bool send_data(int sock, const char* data, size_t length) {
    size_t sent = 0;
    while (sent < length) {
        ssize_t result = send(sock, data + sent, length - sent, 0);
        if (result <= 0) return false;
        sent += result;
    }
    return true;
}

bool read_full(int sock, uint8_t* buffer, size_t length) {
    size_t received = 0;
    while (received < length) {
        ssize_t result = recv(sock, buffer + received, length - received, 0);
        if (result <= 0) return false;
        received += result;
    }
    return true;
}

TEST(ChecksumTest, CalculatesCorrectChecksum) {
    uint8_t data[] = {1, 2, 3, 4, 5};
    EXPECT_EQ(calculate_checksum(data, 5), 15);
}

TEST(ChecksumTest, ZeroLength) {
    uint8_t data[] = {1, 2, 3};
    EXPECT_EQ(calculate_checksum(data, 0), 0);
}

TEST(ChecksumTest, OverflowWrapsAround) {
    uint8_t data[] = {255, 1, 1};
    EXPECT_EQ(calculate_checksum(data, 3), 1);
}

TEST(BigEndianTest, Uint64Conversion) {
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE8};
    EXPECT_EQ(be64toh_custom(data), 1000);
}

TEST(BigEndianTest, Int32Positive) {
    uint8_t data[] = {0x00, 0x00, 0x01, 0x2C};
    EXPECT_EQ(be32toh_signed(data), 300);
}

TEST(BigEndianTest, Int32Negative) {
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFE};
    EXPECT_EQ(be32toh_signed(data), -2);
}

TEST(BigEndianTest, Int16Positive) {
    uint8_t data[] = {0x03, 0xE8};
    EXPECT_EQ(be16toh_signed(data), 1000);
}

TEST(BigEndianTest, Int16Negative) {
    uint8_t data[] = {0xFF, 0xFE};
    EXPECT_EQ(be16toh_signed(data), -2);
}

TEST(BigEndianTest, FloatConversion) {
    uint8_t data[] = {0x41, 0xCC, 0x00, 0x00};
    float result = be32toh_float(data);
    EXPECT_NEAR(result, 25.5f, 0.01f);
}

TEST(TimestampTest, ReturnsNonEmptyString) {
    std::string result = timestamp_to_string(1735257600000000LL);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.length(), 19);
}

TEST(TimestampTest, ZeroTimestamp) {
    std::string result = timestamp_to_string(0);
    EXPECT_FALSE(result.empty());
}

TEST(ErrorHandlingTest, BadChecksum) {
    uint8_t packet[15];
    memset(packet, 0, 15);
    packet[14] = 99;
    EXPECT_NE(calculate_checksum(packet, 14), packet[14]);
}

TEST(ErrorHandlingTest, SendDataWithClosedSocket) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    close(sock);
    EXPECT_FALSE(send_data(sock, "test", 4));
}

TEST(ErrorHandlingTest, ReadFullWithClosedSocket) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    close(sock);
    uint8_t buffer[10];
    EXPECT_FALSE(read_full(sock, buffer, 10));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
EOF

# Makefile
cat > Makefile << 'EOF'
CXX = g++
CXXFLAGS = -std=c++14 -pthread -Wall -Wextra
LDFLAGS = -lgtest -lgtest_main

all: test_data_collector

test_data_collector: test_data_collector.cpp
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@

test: test_data_collector
	./test_data_collector

coverage: CXXFLAGS += --coverage
coverage: LDFLAGS += --coverage
coverage: test_data_collector
	./test_data_collector
	gcov test_data_collector.cpp

clean:
	rm -f test_data_collector *.gcov *.gcda *.gcno

.PHONY: all test coverage clean
EOF

# CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(DataCollectorTests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  test_data_collector
  test_data_collector.cpp
)

target_link_libraries(
  test_data_collector
  GTest::gtest_main
  pthread
)

include(GoogleTest)
gtest_discover_tests(test_data_collector)
EOF

# run_tests.sh
cat > run_tests.sh << 'EOF'
#!/bin/bash

echo "Building tests..."
g++ -std=c++14 -pthread test_data_collector.cpp -lgtest -lgtest_main -o test_data_collector

if [ $? -eq 0 ]; then
    echo "Running tests..."
    ./test_data_collector
else
    echo "Build failed!"
    exit 1
fi
EOF

chmod +x run_tests.sh

echo "Created:"
echo "- test_data_collector.cpp"
echo "- Makefile"
echo "- CMakeLists.txt"
echo "- run_tests.sh"
echo ""
echo "Run with: ./run_tests.sh"