## Компиляция
clang++ -std=c++17 -fprofile-instr-generate -fcoverage-mapping test_cow.cpp -o test_cow.exe


## Запуск тестов 
.\test_cow.exe


## Отчет
llvm-profdata merge default.profraw -o default.profdata

llvm-cov report test_cow.exe --instr-profile=default.profdata


## HTML отчет
llvm-cov show test_cow.exe --instr-profile=default.profdata -format=html -output-dir=coverage_report

start coverage_report\index.html