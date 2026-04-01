# VLSI CAD: Logic to Layout

Programming assignments for the [VLSI CAD: Logic to Layout](https://www.coursera.org/learn/vlsi-cad-layout) course on Coursera, taught by Prof. Rob A. Rutenbar (University of Illinois at Urbana-Champaign).

All implementations are written in **C++23** and make use of the [Boost.DynamicBitset](https://www.boost.org/doc/libs/release/libs/dynamic_bitset/) library where appropriate.

## Assignments

| # | Directory | Topic | Description |
|---|-----------|-------|-------------|
| 1 | [BooleanCalculatorEngine](BooleanCalculatorEngine/) | Boolean Logic | Unate Recursive Complement and a full Boolean Calculator Engine operating on Positional Cube Notation (PCN) |
| 2 | [QuadraticPlacer](QuadraticPlacer/) | Placement | Analytical quadratic placer with recursive partitioning (8x8 deep placer with 64 regions) |
| 3 | [Router](Router/) | Routing | Two-layer maze router with non-unit cell costs, bend penalties, and via support |

## Prerequisites

- A C++23-capable compiler (GCC 14+, Clang 18+, or MSVC 19.38+)
- [Boost](https://www.boost.org/) headers (for `dynamic_bitset`)
- Standard library support for `<print>`, `<generator>`, `<numeric>`, and `<ranges>`

## Building

Each assignment is self-contained in its own directory. Compile with C++23 enabled:

```bash
# Example for BooleanCalculatorEngine
cd BooleanCalculatorEngine
g++ -std=c++23 -O2 -o bce booleanCalculatorEngine.cpp cubeList.cpp

# Example for QuadraticPlacer (requires a Conjugate Gradient solver header)
cd QuadraticPlacer
g++ -std=c++23 -O2 -o placer QuadraticPlacer.cpp
# Usage: ./placer input.txt

# Example for Router
cd Router
g++ -std=c++23 -O2 -o router Router.cpp
```

> **Note:** The QuadraticPlacer depends on a `solver.h` header providing a Conjugate Gradient sparse matrix solver. This solver is distributed as part of the Coursera course starter files and is not included in this repository.

## License

This repository is for educational purposes as part of the Coursera VLSI CAD course.
