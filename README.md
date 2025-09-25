SLAI - Simple Learning Assembly Interpreter
---
Overview
---
SLAI is a simple assembly-like language interpreter written in modern C++. It supports basic assembly instructions, function calls with recursion, conditional jumps, and variables.

Examples of programs implemented on SLAI can be found [here](https://github.com/LincolnCox29/SLAI/tree/main/examples).

Features:
---
- Basic Assembly Instructions: mov, add, sub, mul, div, inc, dec;
- Flow Control: Conditional jumps (je, jne, jg, jl, jge, jle, jmp), labels;
- Functions: call/ret with stack-based execution;
- I/O Operations: msg command for output;

Building
---
1. Build with cmake
```bash
mkdir build
cd build
cmake ..
make --build .
```
2. Add SLAI to PATH

Using
---
```bash
SLAI.exe myAsmFile.asm
```
