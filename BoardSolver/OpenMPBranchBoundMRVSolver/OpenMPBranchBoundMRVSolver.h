#ifndef ADV_AD_A_BRANCH_BOUND_OPENMPBRANCHBOUNDMRVSOLVER_H
#define ADV_AD_A_BRANCH_BOUND_OPENMPBRANCHBOUNDMRVSOLVER_H
#pragma once

#include "../Solver.h"
#include <atomic>

class OpenMPBranchBoundMRVSolver : public Solver {
public:
    explicit OpenMPBranchBoundMRVSolver(int maxThreads = 0);

    void solve(Board &board) override;

private:
    int maxThreads;

    // Parallel phase: spawns an OpenMP task per feasible candidate up to TASK_DEPTH_CUTOFF.
    static bool solveGridParallel(const Board &board, Board &solutionBoard,
                                  std::atomic<bool> &solved, std::atomic<long long> &nodes, int depth);

    // Serial phase: used once task depth exceeds TASK_DEPTH_CUTOFF.
    // Applies MRV + forward checking identically to BranchBoundMRVSolver.
    static bool solveGridSerial(Board &board, std::atomic<bool> &solved, std::atomic<long long> &nodes);

    // Branch + Bound combined: scans all empty cells in one pass.
    // Returns -1 if the board is solved (no empty cells).
    // Returns  0 if a dead-end cell (0 candidates) is found — prune this branch.
    // Returns ≥1 = min candidates found; row/col set to the MRV cell.
    static int selectCell(const Board &board, int &row, int &col);

    // Bound (forward checking): returns false if any empty cell has zero
    // valid candidates, proving this partial assignment cannot be completed.
    static bool isFeasible(const Board &board);

    static int countCandidates(int row, int col, const Board &board);

    static bool isValid(int row, int col, int value, const Board &board);

    static bool isValidInGrid(int startRow, int startCol, int endRow, int endCol,
                              int value, const Board &board);

    static bool isValidInRow(int row, int value, const Board &board);

    static bool isValidInCol(int col, int value, const Board &board);
};

#endif //ADV_AD_A_BRANCH_BOUND_OPENMPBRANCHBOUNDMRVSOLVER_H
