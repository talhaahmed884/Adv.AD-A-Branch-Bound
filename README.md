# Sudoku Solver — Algorithm Analysis & Design (Branch & Bound)

A benchmarking project that implements and compares multiple Sudoku-solving algorithms, from plain serial backtracking to OpenMP-parallelized solvers, with a focus on Branch & Bound strategies. Results are written to CSV for analysis.

---

## Algorithms Implemented

### 1. Serial (Backtracking)
**File:** `BoardSolver/SerialSolver/`

Basic recursive backtracking. Cells are filled in row-major order (top-left to bottom-right). At each empty cell, values 1–9 are tried in sequence; the solver backtracks whenever a conflict is detected.

- **Branch:** next empty cell in row-major order
- **Bound:** reject value if it conflicts with the same row, column, or 3×3 box

### 2. Serial MRV (Backtracking + Heuristic)
**File:** `BoardSolver/SerialMRVSolver/`

Backtracking enhanced with the **Minimum Remaining Values (MRV)** heuristic. Instead of the next cell in order, the solver always picks the empty cell that currently has the fewest valid candidate values. This dramatically reduces the branching factor on hard puzzles.

- **Branch:** cell with fewest remaining valid candidates (MRV)
- **Bound:** reject value if it conflicts with row, column, or box

### 3. Branch & Bound
**File:** `BoardSolver/BranchBoundSolver/`

A clean Branch & Bound implementation with no heuristics. Cells are selected in row-major order. The bounding function checks whether placing a candidate value immediately violates any constraint — if it does, that branch is pruned without recursing. This is the baseline B&B for comparison against the MRV variant.

- **Branch:** next empty cell in row-major order
- **Bound:** prune if the candidate value violates any row, column, or box constraint

### 4. Branch & Bound MRV (Branch & Bound + MRV + Forward Checking)
**File:** `BoardSolver/BranchBoundMRVSolver/`

The full Branch & Bound implementation combining two optimisations:

- **Branch (MRV):** always selects the empty cell with the fewest valid candidates, minimising the branching factor at every node.
- **Bound (Forward Checking):** after placing a value, scans *all* remaining empty cells and prunes immediately if any cell has been left with zero valid candidates. This catches dead ends earlier than plain MRV backtracking, which would only discover the infeasibility when it later visits that cell.

### 5. OpenMP Parallel Solver
**File:** `BoardSolver/OpenMPSolver/`

Parallel solver built on OpenMP tasks. Uses MRV for cell selection. The first few levels of the search tree (controlled by `TASK_DEPTH_CUTOFF = 3`) are expanded in parallel as independent OpenMP tasks; deeper levels fall back to serial MRV backtracking. A shared `atomic<bool>` flag stops all workers once a solution is found.

Benchmarked with **4, 8, and 16 threads**.

### 6. OpenMP Branch & Bound MRV (Parallel + MRV + Forward Checking)
**File:** `BoardSolver/OpenMPBranchBoundMRVSolver/`

Parallel Branch & Bound combining all four techniques: OpenMP task parallelism, MRV cell selection, and forward checking as the bounding function. Key design points:

- **Branch (MRV):** selects the empty cell with fewest valid candidates at every node.
- **Bound (Forward Checking):** after applying a candidate value, `isFeasible()` scans all remaining empty cells. Crucially, this check happens *before* spawning the OpenMP task — branches that fail the bound never enter the task scheduler, reducing task creation overhead compared to checking inside the task.
- **Parallel:** OpenMP tasks up to `TASK_DEPTH_CUTOFF = 3`, then falls back to the serial Branch & Bound MRV solver. A shared `atomic<bool>` flag terminates all workers once a solution is claimed.

Benchmarked with **4, 8, and 16 threads**.

---

## Project Structure

```
.
├── main.cpp
├── CMakeLists.txt
│
├── SudokuBoard/
│   ├── CommonConstants.h          # BoardSize = 9, GridSize = 3, BoardClues = 17
│   ├── Block/                     # Single cell (value + filled flag)
│   └── Board/                     # 9×9 grid of Blocks
│
├── BoardGenerator/
│   ├── BoardGenerator.h/.cpp      # Random puzzle generator (backtracking + uniqueness check)
│   └── PuzzleBank.h               # Pre-loaded puzzles: Easy, Medium, Hard, Extreme
│
├── BoardSolver/
│   ├── Solver.h                       # Abstract base class
│   ├── SerialSolver/
│   ├── SerialMRVSolver/
│   ├── BranchBoundSolver/
│   ├── BranchBoundMRVSolver/
│   ├── OpenMPSolver/
│   └── OpenMPBranchBoundMRVSolver/
│
├── CorrectnessChecker/            # Validates a completed board
│
├── Benchmark/
│   ├── BenchmarkRunner.h/.cpp     # Runs all solvers, writes CSV results
│   └── PuzzleProfiler/            # Counts search nodes for puzzle analysis
│
└── Scripts/
    └── analyze.py                 # Python script for analysing CSV output
```

---

## Dependencies

| Dependency | Purpose | Notes |
|---|---|---|
| C++17 | Language standard | Required for `[[nodiscard]]` and structured bindings |
| CMake ≥ 3.15 | Build system | |
| OpenMP | Parallel solver | Optional — serial fallback is used if not found |
| libomp (macOS) | OpenMP runtime on Apple | Install via `brew install libomp` |

---

## Building

### Standard build (macOS / Linux)

```bash
mkdir cmake-build-debug && cd cmake-build-debug
cmake ..
cmake --build .
```

### macOS with Homebrew libomp

CMake automatically searches `/opt/homebrew/opt/libomp` and `/usr/local/opt/libomp`. If OpenMP is found, it is linked automatically.

### HPC cluster (manual `-fopenmp` flags)

```bash
cmake .. -DHPC=ON
cmake --build .
```

---

## Running

```bash
./Adv_AD_A_Branch_Bound
```

The program runs the full benchmark suite and prints results to stdout as CSV, then writes a timestamped CSV file to the `results/` directory under the project root.

---

## Benchmark Details

### Puzzle difficulties

| Level | Label | Source |
|---|---|---|
| 0 | Easy | `PuzzleBank.h` — `EASY_PUZZLE_BANK` |
| 1 | Medium | `PuzzleBank.h` — `MEDIUM_PUZZLE_BANK` |
| 2 | Hard | `PuzzleBank.h` — `HARD_PUZZLE_BANK` |
| 3 | Extreme | `PuzzleBank.h` — `EXTREME_HARD_PUZZLE_BANK` |

### Configuration (in `BenchmarkRunner.cpp`)

| Constant | Default | Description |
|---|---|---|
| `REPETITIONS` | 5 | Times each puzzle is solved per algorithm |
| `OPENMP_THREAD_COUNTS` | 4, 8, 16 | Thread counts for all parallel solvers |
| `puzzleCount` (arg to `run()`) | 100 | Max puzzles loaded per difficulty level |

### Output CSV columns

| Column | Description |
|---|---|
| `Board_ID` | e.g. `easy_001`, `extreme_042` |
| `Difficulty` | Easy / Medium / Hard / Extreme |
| `Algorithm` | Serial, Serial-MRV, BranchBound, BranchBoundMRV, OpenMP, OMP-BranchBoundMRV |
| `Threads` | Thread count (1 for serial algorithms) |
| `Rep` | Repetition number (1 – REPETITIONS) |
| `Time_s` | Wall-clock time in seconds (9 decimal places) |
| `Nodes` | Number of value placements attempted during the search |
| `Correct` | 1 if the solution passed correctness validation, 0 otherwise |

### Algorithm labels in CSV

| Algorithm | CSV label |
|---|---|
| Serial Backtracking | `Serial` |
| Serial MRV Backtracking | `Serial-MRV` |
| Branch & Bound | `BranchBound` |
| Branch & Bound MRV + Forward Checking | `BranchBoundMRV` |
| OpenMP Parallel (N threads) | `OpenMP` |
| OpenMP Branch & Bound MRV (N threads) | `OMP-BranchBoundMRV` |

---

## Algorithm Comparison

| Algorithm | Cell selection | Bounding function | Parallel |
|---|---|---|---|
| Serial | Row-major order | Constraint check on placement | No |
| Serial-MRV | MRV heuristic | Constraint check on placement | No |
| Branch & Bound | Row-major order | Constraint check on placement | No |
| Branch & Bound MRV | MRV heuristic | Forward checking (all empty cells) | No |
| OpenMP | MRV heuristic | Constraint check on placement | Yes (tasks) |
| OpenMP B&B MRV | MRV heuristic | Forward checking (all empty cells) | Yes (tasks) |

**Serial-MRV vs Branch & Bound MRV:** both use MRV for cell selection, but Serial-MRV only detects a dead end when it actually visits the infeasible cell. Branch & Bound MRV detects it immediately after each placement by checking all remaining empty cells, pruning the branch without an extra recursive call.

**OpenMP vs OpenMP B&B MRV:** both use OpenMP tasks with the same depth cutoff, but OpenMP B&B MRV additionally runs the forward checking bound *before* spawning each task. Infeasible branches are pruned at the spawning loop, so they never enter the task scheduler — reducing task creation overhead on top of reducing the search space.
