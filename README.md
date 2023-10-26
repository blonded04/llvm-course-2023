# llvm-course-2023


### HW-02

For HW-01's solution via LLVM Pass collect executed instructions and / or uses of executed instructions on `-O2` (without phi-instructions). Analyze frequently occurring patterns.

#### Usage

```bash
cd solution
cmake -S . -B build

cd ../

clang++-12 -std=c++17 pass.cpp -c -fPIC -I`llvm-config-12 --includedir` -o pass.o
clang++-12 pass.o -fPIC -shared -o libpass.so
clang-12 ./solution/apps/main.c ./solution/src/life/life.c ./solution/src/sim/sim.c -I./solution/include/ -I./solution/include/life/ -I./solution/include/sim/ -lcsfml-graphics -lcsfml-system -Xclang -load -Xclang ./libpass.so -flegacy-pass-manager
```
