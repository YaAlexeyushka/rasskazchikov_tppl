#include "cow_interpreter.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string captureOutput(Cow::Interpreter& interpreter) {
    std::stringstream buffer;
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());
    interpreter.run();
    std::cout.rdbuf(old);
    return buffer.str();
}

void test_increment() {
    std::ofstream file("test1.cow");
    // Увеличить 3 раза ячейку, вывести как число 3
    file << "MoO MoO MoO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test1.cow");
    std::string output = captureOutput(interpreter);
    if (output == "3\n") {
        std::cout << "test_increment passed\n";
    }
    else {
        std::cout << "test_increment failed\n";
    }
}

void test_loop() {
    std::ofstream file("test2.cow");
    // Установить cell[0]=3, в цикле ходим вправо-влево, меняя ячейки
    file << "MoO MoO MoO MOO moO MoO mOo MOo moo moO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test2.cow");
    std::string output = captureOutput(interpreter);
    if (output == "3\n") {
        std::cout << "test_loop passed\n";
    }
    else {
        std::cout << "test_loop failed\n";
    }
}

void test_character_output() {
    std::ofstream file("test3.cow");
    // Увеличить 65 раз (ASCII 'A'), вывести как символ A
    for (int i = 0; i < 65; i++) file << "MoO ";
    file << "Moo";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test3.cow");
    std::string output = captureOutput(interpreter);
    if (output == "A") {
        std::cout << "test_character_output passed\n";
    }
    else {
        std::cout << "test_character_output failed\n";
    }
}

void test_reset() {
    std::ofstream file("test4.cow");
    // Увеличить 3 раза, сбросить в 0, вывести 0
    file << "MoO MoO MoO OOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test4.cow");
    std::string output = captureOutput(interpreter);
    if (output == "0\n") {
        std::cout << "test_reset passed\n";
    }
    else {
        std::cout << "test_reset failed\n";
    }
}

void test_register_save() {
    std::ofstream file("test6.cow");
    // Установить cell[0]=5, сохранить в регистр, вправо, восстановить из регистра, вывести 5
    file << "MoO MoO MoO MoO MoO MMM moO MMM OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test6.cow");
    std::string output = captureOutput(interpreter);
    if (output == "5\n") {
        std::cout << "test_register_save passed\n";
    }
    else {
        std::cout << "test_register_save failed\n";
    }
}

void test_register_restore() {
    std::ofstream file("test7.cow");
    // Установить cell[0]=3, сохранить в регистр, сбросить cell[0], восстановить из регистра, вывести 3
    file << "MoO MoO MoO MMM OOO MMM OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test7.cow");
    std::string output = captureOutput(interpreter);
    if (output == "3\n") {
        std::cout << "test_register_restore passed\n";
    }
    else {
        std::cout << "test_register_restore failed\n";
    }
}

void test_skip_loop() {
    std::ofstream file("test8.cow");
    // Начало цикла (cell[0]=0, пропускает тело цикла), после цикла увеличить, вывести 1
    file << "MOO MoO MoO MoO moo MoO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test8.cow");
    std::string output = captureOutput(interpreter);
    if (output == "1\n") {
        std::cout << "test_skip_loop passed\n";
    }
    else {
        std::cout << "test_skip_loop failed\n";
    }
}

void test_nested_loops() {
    std::ofstream file("test9.cow");
    // Вложенные циклы: внешний 2 раза, внутренний 2 раза = 4 инкремента в cell[2]
    file << "MoO MoO MOO moO MoO MoO MOO moO MoO mOo MOo moo mOo MOo moo moO moO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test9.cow");
    std::string output = captureOutput(interpreter);
    if (output == "4\n") {
        std::cout << "test_nested_loops passed\n";
    }
    else {
        std::cout << "test_nested_loops failed\n";
    }
}

void test_inst_execute_increment() {
    std::ofstream file("test11.cow");
    // Установить cell[0]=6 (код increment), выполнить mOO (увеличит cell[0] до 7), вывести 7
    file << "MoO MoO MoO MoO MoO MoO mOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test11.cow");
    std::string output = captureOutput(interpreter);
    if (output == "7\n") {
        std::cout << "test_inst_execute_increment passed\n";
    }
    else {
        std::cout << "test_inst_execute_increment failed\n";
    }
}

void test_inst_execute_decrement() {
    std::ofstream file("test12.cow");
    // Установить cell[0]=5 (код decrement), выполнить mOO (уменьшит cell[0] до 4), вывести 4
    file << "MoO MoO MoO MoO MoO mOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test12.cow");
    std::string output = captureOutput(interpreter);
    if (output == "4\n") {
        std::cout << "test_inst_execute_decrement passed\n";
    }
    else {
        std::cout << "test_inst_execute_decrement failed\n";
    }
}

void test_inst_execute_ptr_left() {
    std::ofstream file("test14.cow");
    file << "MoO MoO moO MoO mOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test14.cow");
    std::string output = captureOutput(interpreter);
    if (output == "2\n") {
        std::cout << "test_inst_execute_ptr_left passed\n";
    }
    else {
        std::cout << "test_inst_execute_ptr_left failed\n";
    }
}

void test_inst_execute_ptr_right() {
    std::ofstream file("test14.cow");
    file << "MoO MoO mOO MoO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test14.cow");
    std::string output = captureOutput(interpreter);
    if (output == "1\n") {
        std::cout << "test_inst_execute_ptr_right passed\n";
    }
    else {
        std::cout << "test_inst_execute_ptr_right failed\n";
    }
}

void test_inst_execute_loop_end() {
    std::ofstream file("test15.cow");
    file << "mOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test15.cow");
    std::string output = captureOutput(interpreter);
    if (output == "0\n") {
        std::cout << "test_inst_execute_loop_end passed\n";
    }
    else {
        std::cout << "test_inst_execute_loop_end failed\n";
    }
}

void test_inst_execute_loop_start() {
    std::ofstream file("test17.cow");
    // Установить cell[0]=7, выполнить mOO (loop_start), вывести 7
    file << "MoO MoO MoO MoO MoO MoO MoO mOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test17.cow");
    std::string output = captureOutput(interpreter);
    if (output == "7\n") {  
        std::cout << "test_inst_execute_loop_start passed\n";
    }
    else {
        std::cout << "test_inst_execute_loop_start failed\n";
    }
}

void test_inst_execute_reset() {
    std::ofstream file("test18.cow");
    // Установить cell[0]=8 (код reset), выполнить mOO, вывести 0
    file << "MoO MoO MoO MoO MoO MoO MoO MoO mOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test18.cow");
    std::string output = captureOutput(interpreter);
    if (output == "0\n") {
        std::cout << "test_inst_execute_reset passed\n";
    }
    else {
        std::cout << "test_inst_execute_reset failed\n";
    }
}

void test_inst_execute_register() {
    std::ofstream file("test19.cow");
    // Установить cell[0]=9 (код change_register), выполнить mOO, вывести 9
    file << "MoO MoO MoO MoO MoO MoO MoO MoO MoO mOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test19.cow");
    std::string output = captureOutput(interpreter);
    if (output == "9\n") {
        std::cout << "test_inst_execute_register passed\n";
    }
    else {
        std::cout << "test_inst_execute_register failed\n";
    }
}

void test_inst_execute_stdout() {
    std::ofstream file("test20.cow");
    // Установить cell[0]=10 (код stdout), выполнить mOO (выведет 10)
    file << "MoO MoO MoO MoO MoO MoO MoO MoO MoO MoO mOO";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test20.cow");
    std::string output = captureOutput(interpreter);
    if (output == "10\n") {
        std::cout << "test_inst_execute_stdout passed\n";
    }
    else {
        std::cout << "test_inst_execute_stdout failed\n";
    }
}

void test_inst_execute_invalid() {
    std::ofstream file("test22.cow");
    // Установить cell[0]=99 (неверный код инструкции), выполнить mOO
    for (int i = 0; i < 99; i++) file << "MoO ";
    file << "mOO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test22.cow");
    std::string output = captureOutput(interpreter);
    if (output == "") {
        std::cout << "test_inst_execute_invalid passed\n";
    }
    else {
        std::cout << "test_inst_execute_invalid failed (output: " << output << ")\n";
    }
}

void test_load_nonexistent() {
    Cow::Interpreter interpreter;
    bool result = interpreter.load_program("nonexistent_file_xyz.cow");
    if (!result) {
        std::cout << "test_load_nonexistent passed\n";
    }
    else {
        std::cout << "test_load_nonexistent failed\n";
    }
}

void test_ptr_right_expand() {
    std::ofstream file("test23.cow");
    // Переместить вправо 1005 раз, увеличить, вывести 1
    for (int i = 0; i < 1005; i++) file << "moO ";
    file << "MoO OOM";
    file.close();
    Cow::Interpreter interpreter;
    interpreter.load_program("test23.cow");
    std::string output = captureOutput(interpreter);
    if (output == "1\n") {
        std::cout << "test_ptr_right_expand passed\n";
    }
    else {
        std::cout << "test_ptr_right_expand failed\n";
    }
}

int main() {
    test_increment();
    test_character_output();
    test_loop();
    test_reset();
    test_register_save();
    test_register_restore();
    test_skip_loop();
    test_nested_loops();
    test_inst_execute_increment();
    test_inst_execute_decrement();
    test_inst_execute_ptr_left();
    test_inst_execute_ptr_right();
    test_inst_execute_loop_end();
    test_inst_execute_loop_start();
    test_inst_execute_reset();
    test_inst_execute_register();
    test_inst_execute_stdout();
    test_inst_execute_invalid();
    test_load_nonexistent();
    test_ptr_right_expand();

    return 0;
}