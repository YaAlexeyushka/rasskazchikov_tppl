#include "data_collector.h"
#include <thread>
#include <fstream>
#include <csignal>
#include <iostream>
#include <atomic>

extern std::ofstream data_file;
extern std::atomic<bool> keep_running;

void signal_handler(int signal) {
    std::cout << "\nПолучен сигнал " << signal << ". Завершение работы...\n";
    keep_running = false;
}

int main() {
    signal(SIGINT, signal_handler);   
    signal(SIGTERM, signal_handler);  
    signal(SIGHUP, SIG_IGN);         
    signal(SIGPIPE, SIG_IGN);        
    
    data_file.open("sensor_data.txt", std::ios::app);
    if (!data_file.is_open()) {
        std::ofstream f("sensor_data.txt");
        f << "SOURCE,TIMESTAMP,VALUE1,VALUE2,VALUE3\n";
        f.close();
        data_file.open("sensor_data.txt", std::ios::app);
    }
    
    std::cout << "Запуск сборщика данных... (Ctrl+C для остановки)\n";
    
    std::thread t1(worker_server1);
    std::thread t2(worker_server2);
    
    t1.join();
    t2.join();
    
    if (data_file.is_open()) {
        data_file.close();
    }
    
    std::cout << "Программа завершена.\n";
    
    return 0;
}
