# llvm-course-2023


### HW-02

For HW-01's solution via LLVM Pass collect executed instructions and / or uses of executed instructions on `-O2` (without phi-instructions). Analyze frequently occurring patterns.

#### Usage

```bash
clang++-12 -std=c++17 ./pass/pass.cpp -c -fPIC -I`llvm-config-12 --includedir` -o ./subbuild/pass.o
clang++-12 ./subbuild/pass.o -fPIC -shared -o ./subbuild/libpass.so
clang-12 ./solution/apps/main.c ./solution/src/life/life.c ./solution/src/sim/sim.c ./pass/log.c -O2 -I./solution/include/ -I./solution/include/life/ -I./solution/include/sim/ -lcsfml-graphics -lcsfml-system -Xclang -load -Xclang ./subbuild/libpass.so -flegacy-pass-manager -o ./subbuild/dynamic_pass.out

./subbuild/dynamic_pass.out > ./subbuild/temporary_log.txt

clang++-12 -std=c++17 -O3 -march=native ./pass/log_parser.cpp -o ./subbuild/log_parser.out

./subbuild/log_parser.out < ./subbuild/temporary_log.txt > pass_result.txt
```
