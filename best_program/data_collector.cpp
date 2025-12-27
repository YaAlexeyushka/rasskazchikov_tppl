#include "data_collector.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <csignal>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <endian.h>

std::mutex file_mutex;
std::ofstream data_file;

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

int connect_and_auth(const char* ip, int port, size_t package_size) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;
    
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
    
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(sock);
        return -1;
    }
    
    if (!send_data(sock, "isu_pt", 6)) {
        close(sock);
        return -1;
    }
    
    uint8_t auth_buffer[package_size];
    if (recv(sock, auth_buffer, package_size, 0) <= 0) {
        close(sock);
        return -1;
    }
    
    return sock;
}

void worker_server1() {
    uint8_t buffer[15];
    
    for (;;) {
        int sock = connect_and_auth("95.163.237.76", 5123, 15);
        if (sock < 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        
        for (;;) {
            if (!send_data(sock, "get", 3)) break;
            if (!read_full(sock, buffer, 15)) break;
            
            if (calculate_checksum(buffer, 14) == buffer[14]) {
                file_mutex.lock();
                data_file << "SERVER1," << timestamp_to_string(be64toh_custom(buffer)) << "," 
                          << be32toh_float(buffer + 8) << "," << be16toh_signed(buffer + 12) << "\n";
                data_file.flush();
                file_mutex.unlock();
            } else {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        close(sock);
    }
}

void worker_server2() {
    uint8_t buffer[21];
    
    for (;;) {
        int sock = connect_and_auth("95.163.237.76", 5124, 21);
        if (sock < 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        
        for (;;) {
            if (!send_data(sock, "get", 3)) break;
            if (!read_full(sock, buffer, 21)) break;
            
            if (calculate_checksum(buffer, 20) == buffer[20]) {
                file_mutex.lock();
                data_file << "SERVER2," << timestamp_to_string(be64toh_custom(buffer)) << "," 
                          << be32toh_signed(buffer + 8) << "," << be32toh_signed(buffer + 12) << ","
                          << be32toh_signed(buffer + 16) << "\n";
                data_file.flush();
                file_mutex.unlock();
            } else {
                break;
            }
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        close(sock);
    }
}
