# llvm-course-2023


### HW-02

For HW-01's solution via LLVM Pass collect executed instructions and / or uses of executed instructions on `-O2` (without phi-instructions). Analyze frequently occurring patterns.

#### Usage

```bash
clang++ -std=c++17 pass.cpp -c -fPIC -I`llvm-config --includedir` -o pass.o
clang++ pass.o -fPIC -shared -o libpass.so
clang apps/main.c -lSDL2 -Xclang -load -Xclang libpass.so -flegacy-pass-manager
```
