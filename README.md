# Multi-Pass Compiler & Static Code Analyzer in C

A complete, production-grade compiler pipeline built from scratch in C for a procedural C-like language. The project covers every phase of modern compiler engineering: lexical analysis, formal context-free grammar parsing, semantic verification, symbol table management, and target code generation.

---

## 📌 Features & Architecture

The compilation pipeline operates across five dedicated stages:

1. **Lexical Analysis (Scanner):**
   - Tokenizes raw source files into structured lexical tokens.
   - Robust syntax error handling with precise source code line/column tracking.

2. **Syntax Analysis & AST Construction (Parser):**
   - Implements context-free grammar validation based on formal language specifications.
   - Generates and traverses an Abstract Syntax Tree (AST) representing program hierarchy.

3. **Symbol Table & Scope Management:**
   - Multi-scope hierarchical symbol table (global, local, formal parameters).
   - Fast identifier lookup using collision-handled hash structures.

4. **Semantic Analysis & Type Checking:**
   - Strict static type checking, type inference, and operand compatibility checks.
   - Validates variable declaration bounds, function signature compliance, and loop controls.

5. **Intermediate Representation & Code Generation:**
   - Translates high-level AST constructs into intermediate representation (quads).
   - Generates executable target assembly / virtual machine bytecode instructions.

---

## 🛠 Tech Stack & Tools

- **Core Language:** Pure C (C99/C11 standard)
- **Data Structures:** Hierarchical Symbol Tables, Abstract Syntax Trees (AST), Hash Tables, Dynamic Vectors
- **Build System:** GNU Make / GCC
- **Testing & Debugging:** Valgrind (memory leak detection), GDB

---

## 🚀 Build & Usage

### Prerequisites
- GCC / Clang compiler
- GNU Make

### Compilation

Clone the repository and build the binary:

```bash
# Build the compiler executable
make

# Clean previous build artifacts
make clean
