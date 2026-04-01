# Assignment 2: Quadratic Placer

Implementation of an **analytical quadratic placer** with recursive partitioning for VLSI gate placement on a 100x100 chip surface.

## Overview

The placer minimizes quadratic wirelength by solving systems of linear equations (Ax = b) derived from a clique-based net model. After an initial global placement, a recursive partitioning strategy alternates vertical and horizontal cuts, sorting and assigning gates to sub-regions and re-solving the placement within each partition to spread gates uniformly across the chip. The recursion runs to a depth of 3, producing an 8x8 grid of 64 partitions.

## Algorithm

### Placement Flow

1. **Initial QP** -- Solve the full quadratic placement for all gates on the 100x100 chip.
2. **Recursive Partitioning** (`recursiveQP`, depth=3) -- At each recursion level, `partition` is called three times. Each call toggles the cut direction (`vertical ^= true`), automatically alternating between vertical and horizontal cuts:
   1. **First partition** (vertical) -- Sort gates by (X, Y) using `nth_element`, assign the first half to the left region and the second half to the right, update boundaries using `std::midpoint`, then solve containment QP for both halves.
   2. **Second partition** (horizontal, left half) -- Sort gates by (Y, X), assign top and bottom halves, solve containment QP for both.
   3. **Third partition** (horizontal, right half) -- Same as above for the right sub-region.
   4. **Recurse** on all 4 resulting sub-regions with depth - 1.
3. **Output** -- Sort gates by ID and print coordinates with 8 digits of precision.

This produces 4^3 = 64 leaf regions at the bottom of the recursion, forming the 8x8 grid.

### Partition Step

Each `partition` call performs:
1. **Toggle cut direction** -- Flip `params.vertical ^= true` to alternate between vertical and horizontal cuts.
2. **Assignment** -- `ranges::nth_element` with O(n) partial sort to divide gates at the median along the current cut axis.
3. **Index update** -- Remap gate indices in the net lists to reflect their new positions.
4. **Boundary computation** -- Split the region using `std::midpoint` for correct boundaries at all recursion depths.
5. **Containment QP** -- Solve placement for both sub-regions, propagating connected gates/pads on the opposite side to the cut boundary.

### Containment & Gate Propagation

Gates outside the current partition are propagated to the region boundary:
- **Left/right of region** -- Clamp X to the region boundary, keep Y.
- **Above/below region** -- Keep X, clamp Y to the region boundary.
- **Diagonal** -- Clamp to the nearest corner of the region (`push_to_boundary` via `std::clamp`).

Gates that fall strictly inside the region (between the boundary edges) but belong to the other partition are pushed to the appropriate edge based on the cut direction and which half (lower/upper) is being solved.

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
| `QuadraticPlacer.h` | Class definitions for `QuadraticPlacer`, `Gate`, and `Parameters` (region bounds, cut direction state) |
| `QuadraticPlacer.cpp` | Matrix setup, CG solve, partitioning with `std::midpoint` boundary computation, recursive QP, and I/O |

## Dependencies

This implementation requires a **Conjugate Gradient sparse matrix solver** provided via `solver.h`. The solver uses a COO (Coordinate) sparse matrix format and is distributed as part of the Coursera course starter files.

## Usage

```bash
# Compile (solver.h must be in the include path)
g++ -std=c++23 -O2 -o placer QuadraticPlacer.cpp

# Run the placer
./placer input.txt
```

The program takes a single argument: the input netlist file. It always runs the full 8x8 recursive placer (depth=3, producing 64 partitions) and writes the placement result to stdout.

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
