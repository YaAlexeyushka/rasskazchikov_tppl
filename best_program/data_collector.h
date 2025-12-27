#ifndef DATA_COLLECTOR_H
#define DATA_COLLECTOR_H

#include <string>
#include <cstdint>
#include <cstddef>
#include <atomic>

extern std::atomic<bool> keep_running;  

std::string timestamp_to_string(int64_t timestamp_us);
uint8_t calculate_checksum(const uint8_t* data, size_t length);
uint64_t be64toh_custom(const uint8_t* data);
int32_t be32toh_signed(const uint8_t* data);
int16_t be16toh_signed(const uint8_t* data);
float be32toh_float(const uint8_t* data);
bool send_data(int sock, const char* data, size_t length);
bool read_full(int sock, uint8_t* buffer, size_t length);
int connect_and_auth(const char* ip, int port, size_t package_size);
void worker_server1();
void worker_server2();

#endif 
