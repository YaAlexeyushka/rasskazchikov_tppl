#include "cow_interpreter.h"

int main() {
    Cow::Interpreter interpreter;

    if (interpreter.load_program("input.cow")) {
        interpreter.run();
    }
    else {
        return 1;
    }

    return 0;
}