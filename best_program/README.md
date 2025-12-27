mkdir build && cd build
cmake ..
make
./test_data_collector
cd ~/build
lcov --capture --directory . --output-file coverage.info --ignore-errors inconsistent
lcov --remove coverage.info '/usr/*' '*/gtest/*' '*/test_*' --output-file coverage_filtered.info
genhtml coverage_filtered.info --output-directory coverage_html
xdg-open coverage_html/index.html