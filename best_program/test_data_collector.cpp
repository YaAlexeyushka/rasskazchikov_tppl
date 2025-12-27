#include <gtest/gtest.h>
#include "data_collector.h"
#include <cstring>
#include <endian.h>
#include <sys/socket.h>

class DataCollectorTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DataCollectorTest, TimestampToString) {
    int64_t timestamp = 1609459200000000;
    std::string result = timestamp_to_string(timestamp);
    EXPECT_FALSE(result.empty());
    EXPECT_EQ(result.length(), 19);
}

TEST_F(DataCollectorTest, CalculateChecksumEmpty) {
    uint8_t data[] = {};
    EXPECT_EQ(calculate_checksum(data, 0), 0);
}

TEST_F(DataCollectorTest, CalculateChecksumSingle) {
    uint8_t data[] = {0x05};
    EXPECT_EQ(calculate_checksum(data, 1), 0x05);
}

TEST_F(DataCollectorTest, CalculateChecksumMultiple) {
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    EXPECT_EQ(calculate_checksum(data, 4), 0x0A);
}

TEST_F(DataCollectorTest, CalculateChecksumOverflow) {
    uint8_t data[] = {0xFF, 0xFF, 0xFF};
    EXPECT_EQ(calculate_checksum(data, 3), 0xFD);
}

TEST_F(DataCollectorTest, Be64tohCustom) {
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00};
    uint64_t result = be64toh_custom(data);
    EXPECT_EQ(result, 256);
}

TEST_F(DataCollectorTest, Be64tohCustomZero) {
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(be64toh_custom(data), 0);
}

TEST_F(DataCollectorTest, Be64tohCustomMax) {
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    EXPECT_EQ(be64toh_custom(data), UINT64_MAX);
}

TEST_F(DataCollectorTest, Be32tohSignedPositive) {
    uint8_t data[] = {0x00, 0x00, 0x01, 0x00};
    EXPECT_EQ(be32toh_signed(data), 256);
}

TEST_F(DataCollectorTest, Be32tohSignedNegative) {
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
    EXPECT_EQ(be32toh_signed(data), -1);
}

TEST_F(DataCollectorTest, Be32tohSignedZero) {
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(be32toh_signed(data), 0);
}

TEST_F(DataCollectorTest, Be16tohSignedPositive) {
    uint8_t data[] = {0x01, 0x00};
    EXPECT_EQ(be16toh_signed(data), 256);
}

TEST_F(DataCollectorTest, Be16tohSignedNegative) {
    uint8_t data[] = {0xFF, 0xFF};
    EXPECT_EQ(be16toh_signed(data), -1);
}

TEST_F(DataCollectorTest, Be16tohSignedZero) {
    uint8_t data[] = {0x00, 0x00};
    EXPECT_EQ(be16toh_signed(data), 0);
}

TEST_F(DataCollectorTest, Be32tohFloatPositive) {
    uint8_t data[] = {0x40, 0x49, 0x0F, 0xDB};
    float result = be32toh_float(data);
    EXPECT_NEAR(result, 3.14159f, 0.00001f);
}

TEST_F(DataCollectorTest, Be32tohFloatZero) {
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(be32toh_float(data), 0.0f);
}

TEST_F(DataCollectorTest, Be32tohFloatNegative) {
    uint8_t data[] = {0xC0, 0x49, 0x0F, 0xDB};
    float result = be32toh_float(data);
    EXPECT_NEAR(result, -3.14159f, 0.00001f);
}

TEST_F(DataCollectorTest, SendDataSuccess) {
    int sockets[2];
    ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets), 0);
    
    const char* test_data = "test";
    EXPECT_TRUE(send_data(sockets[0], test_data, 4));
    
    char buffer[5] = {0};
    recv(sockets[1], buffer, 4, 0);
    EXPECT_STREQ(buffer, "test");
    
    close(sockets[0]);
    close(sockets[1]);
}

TEST_F(DataCollectorTest, SendDataFailure) {
    int invalid_sock = -1;
    const char* test_data = "test";
    EXPECT_FALSE(send_data(invalid_sock, test_data, 4));
}

TEST_F(DataCollectorTest, SendDataClosedSocket) {
    int sockets[2];
    ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets), 0);
    close(sockets[1]);
    
    const char* test_data = "test";
    EXPECT_FALSE(send_data(sockets[0], test_data, 4));
    
    close(sockets[0]);
}

TEST_F(DataCollectorTest, ReadFullSuccess) {
    int sockets[2];
    ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets), 0);
    
    const char* test_data = "test";
    send(sockets[0], test_data, 4, 0);
    
    uint8_t buffer[4];
    EXPECT_TRUE(read_full(sockets[1], buffer, 4));
    EXPECT_EQ(memcmp(buffer, "test", 4), 0);
    
    close(sockets[0]);
    close(sockets[1]);
}

TEST_F(DataCollectorTest, ReadFullFailure) {
    int invalid_sock = -1;
    uint8_t buffer[4];
    EXPECT_FALSE(read_full(invalid_sock, buffer, 4));
}

TEST_F(DataCollectorTest, ReadFullPartialData) {
    int sockets[2];
    ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, sockets), 0);
    
    const char* test_data = "te";
    send(sockets[0], test_data, 2, 0);
    close(sockets[0]);
    
    uint8_t buffer[4];
    EXPECT_FALSE(read_full(sockets[1], buffer, 4));
    
    close(sockets[1]);
}

TEST_F(DataCollectorTest, Server1PacketValid) {
    uint8_t packet[15];
    packet[0] = 0x00; packet[1] = 0x05; packet[2] = 0xB3;
    packet[3] = 0x77; packet[4] = 0xD6; packet[5] = 0x78;
    packet[6] = 0x00; packet[7] = 0x00;
    packet[8] = 0x40; packet[9] = 0x49; packet[10] = 0x0F; packet[11] = 0xDB;
    packet[12] = 0x00; packet[13] = 0x64;
    packet[14] = calculate_checksum(packet, 14);
    
    EXPECT_EQ(calculate_checksum(packet, 14), packet[14]);
    EXPECT_NEAR(be32toh_float(packet + 8), 3.14159f, 0.00001f);
    EXPECT_EQ(be16toh_signed(packet + 12), 100);
}

TEST_F(DataCollectorTest, Server2PacketValid) {
    uint8_t packet[21];
    packet[0] = 0x00; packet[1] = 0x05; packet[2] = 0xB3;
    packet[3] = 0x77; packet[4] = 0xD6; packet[5] = 0x78;
    packet[6] = 0x00; packet[7] = 0x00;
    packet[8] = 0x00; packet[9] = 0x00; packet[10] = 0x03; packet[11] = 0xE8;
    packet[12] = 0xFF; packet[13] = 0xFF; packet[14] = 0xFE; packet[15] = 0x0C;
    packet[16] = 0x00; packet[17] = 0x00; packet[18] = 0x07; packet[19] = 0xD0;
    packet[20] = calculate_checksum(packet, 20);
    
    EXPECT_EQ(calculate_checksum(packet, 20), packet[20]);
    EXPECT_EQ(be32toh_signed(packet + 8), 1000);
    EXPECT_EQ(be32toh_signed(packet + 12), -500);
    EXPECT_EQ(be32toh_signed(packet + 16), 2000);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
