# Assignment 2: Quadratic Placer

Implementation of an **analytical quadratic placer** with recursive partitioning for VLSI gate placement on a 100x100 chip surface.

## Overview

The placer minimizes quadratic wirelength by solving systems of linear equations (Ax = b) derived from a clique-based net model. After each global placement, a recursive partitioning strategy sorts and assigns gates to sub-regions, then re-solves the placement within each partition to spread gates uniformly across the chip.

## Algorithm

### Core 3QP Placer

The required assignment implements exactly 3 quadratic placement solves:

1. **QP1** -- Solve the full placement problem for all gates on the 100x100 chip.
2. **Sort & Assign** -- Sort gates by (X, Y) coordinates and assign the first half to the left partition (X: 0-50) and the second half to the right partition (X: 50-100). If the gate count is odd, the left side gets the smaller half.
3. **Containment** -- For each partition, propagate connected gates/pads on the opposite side to the cut line (X = 50) while preserving their Y coordinates.
4. **QP2** -- Solve placement for left-side gates within [0, 50] x [0, 100].
5. **QP3** -- Solve placement for right-side gates within [50, 100] x [0, 100], using updated positions from QP2 for propagated left-side gates.

### 8x8 Deep Placer (Extra Credit)

Extends the 3QP strategy with alternating vertical and horizontal cuts recursively down to an 8x8 grid of 64 partitions. The `recursiveQP` method handles this by partitioning each region into sub-regions and recursing until the desired depth is reached.

Gate/pad propagation in the general case handles three scenarios:
- **Left/right of region** -- Clamp X to the region boundary, keep Y.
- **Above/below region** -- Keep X, clamp Y to the region boundary.
- **Diagonal** -- Clamp to the nearest corner of the region.

## Input Format

```
<num_gates> <num_nets>
<gate_id> <num_connected_nets> <net_id_1> <net_id_2> ...
...
<num_pads>
<pad_id> <net_id> <x_coord> <y_coord>
...
```

## Output Format

One line per gate, sorted by gate ID:

```
<gate_id> <x_coordinate_8dp> <y_coordinate_8dp>
```

## Source Files

| File | Description |
|------|-------------|
| `QuadraticPlacer.h` | Class definitions for `QuadraticPlacer`, `Gate`, and `Parameters` (partition boundaries and state) |
| `QuadraticPlacer.cpp` | Matrix setup, Conjugate Gradient solve, partitioning, recursive QP, and I/O |

## Dependencies

This implementation requires a **Conjugate Gradient sparse matrix solver** provided via `solver.h`. The solver uses a COO (Coordinate) sparse matrix format and is distributed as part of the Coursera course starter files.

## Usage

```bash
# Compile (solver.h must be in the include path)
g++ -std=c++23 -O2 -o placer QuadraticPlacer.cpp

# Run 3QP placer (depth = 0)
./placer input.txt 0

# Run 8x8 deep placer (depth = 2 gives 8x8 = 64 regions)
./placer input.txt 2
```

The second argument controls recursion depth: `0` for the core 3QP placer, `2` for the full 8x8 placer.

## Benchmarks

### 3QP (Required)

| Benchmark | Gates | Nets | Pads |
|-----------|-------|------|------|
| Toy1 | 18 | 20 | 6 |
| Toy2 | 32 | 42 | 10 |
| Fract | 125 | 147 | 24 |
| Primary1 | 752 | 902 | 107 |
| Struct | 1888 | 1920 | 64 |

### 8x8 (Extra Credit)

| Benchmark | Gates | Nets | Pads |
|-----------|-------|------|------|
| Industry1 | 2271 | 2478 | 490 |
| Biomed | 6417 | 5742 | 97 |
| Industry2 | 12142 | 13419 | 495 |

## Grading Criteria

- **Structural correctness** (20%) -- All gates present and within the 100x100 chip area.
- **HPWL wirelength** (40%) -- Total Half-Perimeter Wirelength compared to the reference solution.
- **Balanced placement** (40%) -- Roughly equal gate distribution across partitions.
