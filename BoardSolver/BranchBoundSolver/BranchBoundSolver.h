#ifndef ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDSOLVER_H
#define ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDSOLVER_H
#pragma once

#include "../Solver.h"

class BranchBoundSolver : public Solver {
public:
    BranchBoundSolver();

    void solve(Board &board) override;

private:
    static bool solveGrid(Board &board);

    // Branch: select the next empty cell in row-major order.
    // Returns false when no empty cell remains (board is solved).
    static bool selectCell(const Board &board, int &row, int &col);

    // Bound: returns false if placing value at (row, col) violates any
    // row, column, or box constraint — pruning that branch immediately.
    static bool isFeasible(int row, int col, int value, const Board &board);

    static bool isValidInGrid(int startRow, int startCol, int endRow, int endCol,
                              int value, const Board &board);

    static bool isValidInRow(int row, int value, const Board &board);

    static bool isValidInCol(int col, int value, const Board &board);
};

#endif //ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDSOLVER_H
