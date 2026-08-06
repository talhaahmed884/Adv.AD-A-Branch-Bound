#include "BranchBoundMRVSolver.h"

BranchBoundMRVSolver::BranchBoundMRVSolver() = default;

void BranchBoundMRVSolver::solve(Board &board) {
    nodeCount = 0;
    solveGrid(board, nodeCount);
}

bool BranchBoundMRVSolver::solveGrid(Board &board, long long &nodes) {
    int row = -1;
    int col = -1;

    // Combined branch + bound: one scan finds the MRV cell and detects dead ends.
    const int minCand = selectCell(board, row, col);
    if (minCand == -1) return true;  // no empty cells — solved
    if (minCand == 0)  return false; // dead-end cell found — prune

    for (int val = 1; val <= static_cast<int>(CommonConstants::BoardSize); val++) {
        if (!isValid(row, col, val, board))
            continue;

        board.setBoardValue(row, col, val);
        ++nodes;

        if (solveGrid(board, nodes))
            return true;

        board.resetBoardBlock(row, col);
    }

    return false;
}

int BranchBoundMRVSolver::selectCell(const Board &board, int &row, int &col) {
    constexpr int boardSize = static_cast<int>(CommonConstants::BoardSize);
    int minCandidates = boardSize + 1;
    row = -1;
    col = -1;

    for (int r = 0; r < boardSize; r++) {
        for (int c = 0; c < boardSize; c++) {
            if (board.getBoardBlock(r, c).getIsFilled()) continue;

            const int cand = countCandidates(r, c, board);
            if (cand < minCandidates) {
                minCandidates = cand;
                row = r;
                col = c;
                if (minCandidates == 0) return 0; // dead-end cell: prune without scanning further
            }
        }
    }

    return (row == -1) ? -1 : minCandidates;
}

int BranchBoundMRVSolver::countCandidates(const int row, const int col, const Board &board) {
    int count = 0;
    for (int val = 1; val <= static_cast<int>(CommonConstants::BoardSize); val++) {
        if (isValid(row, col, val, board)) count++;
    }
    return count;
}

bool BranchBoundMRVSolver::isValid(const int row, const int col, const int value, const Board &board) {
    constexpr int gridSize = static_cast<int>(CommonConstants::GridSize);

    const int startRow = (row / gridSize) * gridSize;
    const int startCol = (col / gridSize) * gridSize;
    const int endRow   = startRow + gridSize;
    const int endCol   = startCol + gridSize;

    return isValidInGrid(startRow, startCol, endRow, endCol, value, board) &&
           isValidInRow(row, value, board) &&
           isValidInCol(col, value, board);
}

bool BranchBoundMRVSolver::isValidInGrid(const int startRow, const int startCol,
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

bool BranchBoundMRVSolver::isValidInRow(const int row, const int value, const Board &board) {
    for (int c = 0; c < static_cast<int>(CommonConstants::BoardSize); c++) {
        if (board.getBoardValue(row, c) == value)
            return false;
    }
    return true;
}

bool BranchBoundMRVSolver::isValidInCol(const int col, const int value, const Board &board) {
    for (int r = 0; r < static_cast<int>(CommonConstants::BoardSize); r++) {
        if (board.getBoardValue(r, col) == value)
            return false;
    }
    return true;
}
