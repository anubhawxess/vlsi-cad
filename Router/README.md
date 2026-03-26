# Assignment 3: Maze Router

Implementation of a **two-layer maze router** using Dijkstra-based wavefront expansion with support for non-unit cell costs, bend penalties, and via transitions between layers.

## Overview

The router reads a grid description (with obstacles and non-uniform cell costs) and a netlist of 2-pin nets. It routes each net sequentially using a priority-queue-driven maze search, marking routed cells as obstacles for subsequent nets.

## Algorithm

### Maze Routing (Dijkstra Wavefront Expansion)

For each net:

1. **Initialize** -- Push the source cell onto a min-heap, keyed by cumulative path cost.
2. **Expand** -- Pop the lowest-cost cell. For each unreached neighbor (N/S/E/W on the same layer, or via to the other layer):
   - Add the neighbor's cell cost to the path cost.
   - If the expansion changes direction (N/S to E/W or vice versa), add the bend penalty.
   - If the expansion crosses layers via a via, add the via penalty.
   - Push the neighbor onto the heap.
3. **Terminate** -- When the target cell is popped, perform a backtrace from target to source using stored predecessor directions.
4. **Mark** -- All cells on the routed path are marked as obstacles (cost = -1) to prevent future nets from crossing them.

### Supported Features

| Feature | Description |
|---------|-------------|
| Non-unit cell costs | Grid cells can have arbitrary positive integer routing costs |
| Obstacles | Cells with cost = -1 are blocked and cannot be routed through |
| Bend penalty | Added when the routing path changes from horizontal to vertical or vice versa |
| Via penalty | Added when the path transitions between Layer 1 and Layer 2 |
| Two routing layers | Full support for nets with pins on either or both layers |

## Input Formats

### Grid File (`.grid`)

```
<X_gridsize> <Y_gridsize> <BendPenalty> <ViaPenalty>
<X*Y integers for Layer 1 costs, row by row (y=0 first)>
<X*Y integers for Layer 2 costs, row by row (y=0 first)>
```

Cell cost values: positive integer = routable with that cost, -1 = obstacle.

### Netlist File (`.nl`)

```
<NetNumber>
<NetID> <LayerPin1> <Xpin1> <Ypin1> <LayerPin2> <Xpin2> <Ypin2>
...
```

Nets are routed in the order they appear (by NetID).

## Output Format

```
<NetNumber>
<NetID>
<LayerInfo> <X> <Y>       # 1=Layer1, 2=Layer2, 3=Via
...
0                          # end-of-net marker
<NetID>
...
```

An unrouted net is indicated by a `0` immediately after the NetID line.

## Source Files

| File | Description |
|------|-------------|
| `Router.h` | `Router` class with grid storage, `Cell` struct, direction enum, and `Coordinates` type alias |
| `Router.cpp` | Wavefront expansion, neighbor generation (using C++23 `std::generator`), backtrace, and I/O |

## Usage

```bash
# Compile
g++ -std=c++23 -O2 -o router Router.cpp

# Route a benchmark (reads <name>.grid and <name>.nl)
./router benchmark_name
```

The argument is the base filename without extension. The router reads `<name>.grid` and `<name>.nl`, then writes the routing result to stdout.

## Path Cost Calculation Example

For a path starting on Layer 1 at (0,1) and ending on Layer 2 at (5,3), with a via at (4,1):

```
Layer 1: 1 + 1 + 1 + 4           = 7   (cell costs)
Via:                              + 10  (via penalty)
Layer 2: 1 + 1 + 6               = 8   (cell costs)
Bend:                             + 5   (one direction change)
Layer 2: 5 + 5                    = 10  (remaining cells)
                                  ----
Total:                              40
```

## Benchmarks

| Benchmark | Description | Points |
|-----------|-------------|--------|
| 1-Layer Unit-Cost | 20 nets, simple patterns, Layer 1 only | 25 |
| 1-Layer Bend+Cost | 20 nets, non-uniform costs, Layer 1 only | 25 |
| No-Via 2-Layer | 16 nets (8 per layer), no layer crossings needed | 20 |
| Real 2-Layer | 15 nets, some requiring via transitions | 15 |
| Industrial (fract) | 128 nets from a placed MCNC benchmark, 2 layers | 15 |

## Grading Criteria

- **Completion** -- Successfully routed nets.
- **Cost** -- Path cost compared to the reference router (full credit within 2x, partial within 4x/8x).
- **Shape** -- Correct avoidance of obstacles, minimal detours, proper bend handling.
