#ifndef ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDMRVSOLVER_H
#define ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDMRVSOLVER_H
#pragma once

#include "../Solver.h"

class BranchBoundMRVSolver : public Solver {
public:
    BranchBoundMRVSolver();

    void solve(Board &board) override;

private:
    static bool solveGrid(Board &board);

    // Branch: MRV cell selection — pick the empty cell with fewest valid candidates.
    // Returns false when no empty cell remains (board is solved).
    static bool selectCell(const Board &board, int &row, int &col);

    // Bound (forward checking): returns false if any empty cell has zero valid
    // candidates, proving this partial assignment cannot be completed.
    static bool isFeasible(const Board &board);

    static int countCandidates(int row, int col, const Board &board);

    static bool isValid(int row, int col, int value, const Board &board);

    static bool isValidInGrid(int startRow, int startCol, int endRow, int endCol,
                              int value, const Board &board);

    static bool isValidInRow(int row, int value, const Board &board);

    static bool isValidInCol(int col, int value, const Board &board);
};

#endif //ADV_AD_A_BRANCH_BOUND_BRANCHBOUNDMRVSOLVER_H
