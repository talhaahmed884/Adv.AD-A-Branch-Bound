#include "BranchBoundSolver.h"

BranchBoundSolver::BranchBoundSolver() = default;

void BranchBoundSolver::solve(Board &board) {
    solveGrid(board);
}

bool BranchBoundSolver::solveGrid(Board &board) {
    int row = -1;
    int col = -1;

    // Branch: pick the next empty cell in row-major order
    if (!selectCell(board, row, col))
        return true;

    for (int val = 1; val <= static_cast<int>(CommonConstants::BoardSize); val++) {
        // Bound: prune if placing val here violates a constraint
        if (!isFeasible(row, col, val, board))
            continue;

        board.setBoardValue(row, col, val);

        if (solveGrid(board))
            return true;

        board.resetBoardBlock(row, col);
    }

    return false;
}

bool BranchBoundSolver::selectCell(const Board &board, int &row, int &col) {
    constexpr int boardSize = static_cast<int>(CommonConstants::BoardSize);

    for (int r = 0; r < boardSize; r++) {
        for (int c = 0; c < boardSize; c++) {
            if (!board.getBoardBlock(r, c).getIsFilled()) {
                row = r;
                col = c;
                return true;
            }
        }
    }

    return false;
}

bool BranchBoundSolver::isFeasible(const int row, const int col, const int value, const Board &board) {
    constexpr int gridSize = static_cast<int>(CommonConstants::GridSize);

    const int startRow = (row / gridSize) * gridSize;
    const int startCol = (col / gridSize) * gridSize;
    const int endRow   = startRow + gridSize;
    const int endCol   = startCol + gridSize;

    return isValidInRow(row, value, board) &&
           isValidInCol(col, value, board) &&
           isValidInGrid(startRow, startCol, endRow, endCol, value, board);
}

bool BranchBoundSolver::isValidInGrid(const int startRow, const int startCol,
                                      const int endRow, const int endCol,
                                      const int value, const Board &board) {
    for (int r = startRow; r < endRow; r++) {
        for (int c = startCol; c < endCol; c++) {
            if (board.getBoardValue(r, c) == value)
                return false;
        }
    }
    return true;
}

bool BranchBoundSolver::isValidInRow(const int row, const int value, const Board &board) {
    for (int c = 0; c < static_cast<int>(CommonConstants::BoardSize); c++) {
        if (board.getBoardValue(row, c) == value)
            return false;
    }
    return true;
}

bool BranchBoundSolver::isValidInCol(const int col, const int value, const Board &board) {
    for (int r = 0; r < static_cast<int>(CommonConstants::BoardSize); r++) {
        if (board.getBoardValue(r, col) == value)
            return false;
    }
    return true;
}
