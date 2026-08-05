#ifndef ADV_AD_A_BRANCH_BOUND_SOLVER_H
#define ADV_AD_A_BRANCH_BOUND_SOLVER_H
#pragma once

#include "../SudokuBoard/Board/Board.h"

class Solver {
public:
    virtual ~Solver() = default;

    virtual void solve(Board &board) = 0;
};

#endif //ADV_AD_A_BRANCH_BOUND_SOLVER_H
