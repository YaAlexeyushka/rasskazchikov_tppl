#ifndef COW_INTERPRETER_H
#define COW_INTERPRETER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <optional>

namespace Cow {
    class Interpreter {
        private:
            int ptr = 0;
            int instruction_num = 0;
            std::vector<int> memory;
            std::optional<int> register1;
            std::vector<std::string> instructions;

            void ptr_left() {
                if (ptr > 0) {
                    ptr--;
                }
            }

            void ptr_right() {
                ptr++;
                if (ptr >= memory.size()) {
                    memory.resize(ptr + 1, 0);
                }
            }

            void memory_cell_reset() {
                memory[ptr] = 0;
            }

            void cell_incr() {
                memory[ptr]++;
            }

            void cell_decr() {
                memory[ptr]--;
            }

            void stdin_input() {
                std::cin >> memory[ptr];
            }

            void change_register() {
                if (register1.has_value()) {
                    memory[ptr] = register1.value();
                    register1 = std::nullopt;
                    return;
                }
                register1 = memory[ptr];
            }

            void loop_start() {
                int depth = 1; 
                if (memory[ptr] == 0) {
                    instruction_num++;
                    while (instruction_num < instructions.size()) {
                        if (instructions[instruction_num] == "MOO") {
                            depth++;
                        }
                        else if (instructions[instruction_num] == "moo") {
                            depth--;
                            if (depth == 0) {
                                return;
                            }
                        }
                        instruction_num++;
                    }
                }
            }

            void loop_end() {
                int depth = 1; 
                if (memory[ptr] != 0) {
                    instruction_num--;
                    while (instruction_num >= 0) {
                        if (instructions[instruction_num] == "moo") {
                            depth++;
                        }
                        else if (instructions[instruction_num] == "MOO") {
                            depth--;
                            if (depth == 0) {
                                instruction_num--;
                                return;
                            }
                        }
                        instruction_num--;
                    }
                }
            }

            void char_input_output() {
                if (memory[ptr] == 0) {
                    char ch;
                    std::cin >> ch;
                    memory[ptr] = static_cast<int>(ch);
                    return;
                }
                std::cout << static_cast<char>(memory[ptr]);
            }

            void stdout_output() {
                std::cout << memory[ptr] << std::endl;
            }

            bool inst_execute() {
                switch (memory[ptr]) {
                    case 0:
                        loop_end();
                        return true;
                    case 1:
                        ptr_left();
                        return true;
                    case 2:
                        ptr_right();
                        return true;
                    case 4:
                        char_input_output();
                        return true;
                    case 5:
                        cell_decr();
                        return true;
                    case 6:
                        cell_incr();
                        return true;
                    case 7:
                        loop_start();
                        return true;
                    case 8:
                        memory_cell_reset();
                        return true;
                    case 9:
                        change_register();
                        return true;
                    case 10:
                        stdout_output();
                        return true;
                    case 11:
                        stdin_input();
                        return true;
                    default:
                        return false;    
                }
            }

        public:
            Interpreter() : memory(1000, 0) {}

            bool load_program(const std::string& file_name) {
                std::ifstream file(file_name);
                if (!file.is_open()) {
                    std::cerr << "Error reading file: " << file_name << std::endl;
                    return false;
                }

                std::string x;
                while (file >> x) {
                    if (x.size() == 3) {
                        instructions.push_back(x);
                    }
                }
                return true;
            }

            void run() {
                instruction_num = 0;
                
                while (instruction_num < instructions.size()) {
                    const std::string& inst = instructions[instruction_num];
                    
                    if (inst == "moo") {
                        loop_end();
                    }
                    else if (inst == "mOo") {
                        ptr_left();
                    }
                    else if (inst == "moO") {
                        ptr_right();
                    }
                    else if (inst == "OOO") {
                        memory_cell_reset();
                    }
                    else if (inst == "MoO") {
                        cell_incr();
                    }
                    else if (inst == "MOo") {
                        cell_decr();
                    }
                    else if (inst == "Moo") {
                        char_input_output();
                    }
                    else if (inst == "MMM") {
                        change_register();
                    }
                    else if (inst == "mOO") {
                        if (!inst_execute()) {
                            return;
                        }
                    } 
                    else if (inst == "MOO") {
                        loop_start();
                    }
                    else if (inst == "OOM") {
                        stdout_output();
                    }
                    else if (inst == "oom") {
                        stdin_input();
                    }
                    instruction_num++;
                }
            }
    };
} 

#endif 