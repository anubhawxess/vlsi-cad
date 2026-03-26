# Assignment 1: Boolean Calculator Engine

Implementation of the **Unate Recursive Complement** algorithm and a complete **Boolean Calculator Engine** that operates on Boolean functions represented in Positional Cube Notation (PCN).

## Overview

The program has two modes of operation:

1. **Complement mode** -- Given a PCN file describing a Boolean function F, compute and output F' (the complement).
2. **Calculator mode** -- Read a command file that orchestrates reads, Boolean operations (NOT, AND, OR), and writes across multiple named Boolean functions.

Both modes use the Unate Recursive Paradigm (URP) as the core computational engine.

## Algorithm

### Unate Recursive Complement

```
Complement(F):
  if F is empty             -> return the all-don't-care cube (F' = 1)
  if F contains all-DC cube -> return empty list           (F' = 0)
  if F has exactly one cube -> apply DeMorgan's Law directly
  else:
    x = most binate splitting variable
    P = Complement(positiveCofactor(F, x))
    N = Complement(negativeCofactor(F, x))
    return OR(AND(x, P), AND(x', N))
```

### Splitting Variable Selection

1. Pick the most binate variable (appears in both true and complement form in the most cubes).
2. Break ties by smallest |T - C| (where T = true-form count, C = complement-form count).
3. Break further ties by lowest variable index.
4. If no binate variable exists, pick the unate variable appearing in the most cubes (ties broken by lowest index).

### Boolean Calculator Operations

| Command | Syntax | Operation |
|---------|--------|-----------|
| Read | `r n` | Read file `n.pcn` into function F_n |
| NOT | `! k n` | F_k = complement of F_n |
| OR | `+ k n m` | F_k = F_n + F_m (cube list concatenation) |
| AND | `& k n m` | F_k = F_n * F_m (via DeMorgan: `NOT(OR(NOT(F_n), NOT(F_m)))`) |
| Print | `p n` | Write F_n to stdout in PCN format |
| Quit | `q` | Stop processing |

## PCN File Format

```
<number_of_variables>
<number_of_cubes>
<count_of_non_DC_vars> <var1> <var2> ...    # positive int = true form, negative = complement
...
```

**Example** -- F(x1..x6) = x2·x4·x5' + x2'·x4'·x6 + x1·x2·x3'·x4' + x5·x6:

```
6
4
3 2 4 -5
3 -2 -4 6
4 1 2 -3 -4
2 5 6
```

## Source Files

| File | Description |
|------|-------------|
| `cubeList.h` | `cubeList` class definition using `boost::dynamic_bitset` for compact PCN storage |
| `cubeList.cpp` | Printing, direct complement (termination cases), and splitting variable selection |
| `booleanCalculatorEngine.cpp` | URP complement recursion, cofactors, AND/OR, file I/O, and the calculator command loop |

## Usage

```bash
# Compile
g++ -std=c++23 -O2 -o bce booleanCalculatorEngine.cpp cubeList.cpp

# Complement a single PCN file
./bce input.pcn

# Run the Boolean calculator with a command file
./bce commands.txt
```

The program detects the mode automatically: if the argument ends with `.pcn`, it runs in complement mode; otherwise it interprets the file as a calculator command script.

## Constraints

- Up to 20 variables
- Up to 2^20 cubes per function
- Up to 32 named functions (F0 through F31) in calculator mode
