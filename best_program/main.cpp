#include "data_collector.h"
#include <thread>
#include <fstream>
#include <csignal>

extern std::ofstream data_file;

int main() {
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    
    data_file.open("sensor_data.txt", std::ios::app);
    if (!data_file.is_open()) {
        std::ofstream f("sensor_data.txt");
        f << "SOURCE,TIMESTAMP,VALUE1,VALUE2,VALUE3\n";
        f.close();
        data_file.open("sensor_data.txt", std::ios::app);
    }
    
    std::thread t1(worker_server1);
    std::thread t2(worker_server2);
    
    t1.join();
    t2.join();
    
    return 0;
}
