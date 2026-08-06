#ifndef ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDMRVSOLVER_H
#define ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDMRVSOLVER_H
#pragma once

#include "../Solver.h"

class BranchBoundMRVSolver : public Solver {
public:
    BranchBoundMRVSolver();

    void solve(Board &board) override;

private:
    static bool solveGrid(Board &board, long long &nodes);

    // Branch + Bound combined: scans all empty cells in one pass.
    // Returns -1 if the board is solved (no empty cells).
    // Returns  0 if a dead-end cell (0 candidates) is found — prune this branch.
    // Returns ≥1 = min candidates found; row/col set to the MRV cell.
    static int selectCell(const Board &board, int &row, int &col);

    static int countCandidates(int row, int col, const Board &board);

    static bool isValid(int row, int col, int value, const Board &board);

    static bool isValidInGrid(int startRow, int startCol, int endRow, int endCol,
                              int value, const Board &board);

    static bool isValidInRow(int row, int value, const Board &board);

    static bool isValidInCol(int col, int value, const Board &board);
};

#endif //ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDMRVSOLVER_H
