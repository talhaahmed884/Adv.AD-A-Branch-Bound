#include "OpenMPBranchBoundMRVSolver.h"

#ifdef _OPENMP
#include <omp.h>
#endif

static constexpr int TASK_DEPTH_CUTOFF = 3;

OpenMPBranchBoundMRVSolver::OpenMPBranchBoundMRVSolver(const int maxThreads) : maxThreads(maxThreads) {
#ifdef _OPENMP
    if (this->maxThreads > 0)
        omp_set_num_threads(this->maxThreads);
#endif
}

void OpenMPBranchBoundMRVSolver::solve(Board &board) {
    std::atomic<bool> solved(false);
    std::atomic<long long> nodes(0);
    Board solutionBoard;

#ifdef _OPENMP
#pragma omp parallel
    {
#pragma omp single nowait
        {
            solveGridParallel(board, solutionBoard, solved, nodes, 0);
        }
    }
    if (solved.load())
        board = solutionBoard;
#else
    solveGridSerial(board, solved, nodes);
#endif
    nodeCount = nodes.load();
}

bool OpenMPBranchBoundMRVSolver::solveGridParallel(const Board &board, Board &solutionBoard,
                                                    std::atomic<bool> &solved, std::atomic<long long> &nodes,
                                                    const int depth) {
    if (solved.load()) return false;

    int row = -1;
    int col = -1;

    const int minCand = selectCell(board, row, col);
    if (minCand == -1) {
#pragma omp critical
        {
            if (!solved.exchange(true))
                solutionBoard = board;
        }
        return true;
    }
    if (minCand == 0) return false;

    // Each task works from its own snapshot; `board` is never written from here.
    const Board snapshot = board;

#pragma omp taskgroup
    {
        for (int val = 1; val <= static_cast<int>(CommonConstants::BoardSize); val++) {
            if (solved.load()) break;
            if (!isValid(row, col, val, snapshot)) continue;

            // Apply the candidate and run the bounding function before spawning.
            // Branches that fail forward checking are pruned here, avoiding task overhead.
            Board candidate = snapshot;
            candidate.setBoardValue(row, col, val);
            nodes.fetch_add(1, std::memory_order_relaxed);
            if (!isFeasible(candidate)) continue;

#pragma omp task firstprivate(depth, candidate) shared(solutionBoard, solved, nodes)
            {
                if (!solved.load()) {
                    if (depth + 1 < TASK_DEPTH_CUTOFF) {
                        solveGridParallel(candidate, solutionBoard, solved, nodes, depth + 1);
                    } else {
                        if (solveGridSerial(candidate, solved, nodes)) {
#pragma omp critical
                            {
                                if (!solved.exchange(true))
                                    solutionBoard = candidate;
                            }
                        }
                    }
                }
            }
        }
    }

    return solved.load();
}

bool OpenMPBranchBoundMRVSolver::solveGridSerial(Board &board, std::atomic<bool> &solved,
                                                   std::atomic<long long> &nodes) {
    if (solved.load()) return false;

    int row = -1;
    int col = -1;

    // Combined branch + bound in one scan.
    const int minCand = selectCell(board, row, col);
    if (minCand == -1) return true;  // solved
    if (minCand == 0)  return false; // dead-end cell — prune

    for (int val = 1; val <= static_cast<int>(CommonConstants::BoardSize); val++) {
        if (solved.load()) return false;
        if (!isValid(row, col, val, board)) continue;

        board.setBoardValue(row, col, val);
        nodes.fetch_add(1, std::memory_order_relaxed);

        if (solveGridSerial(board, solved, nodes))
            return true;

        board.resetBoardBlock(row, col);
    }

    return false;
}

int OpenMPBranchBoundMRVSolver::selectCell(const Board &board, int &row, int &col) {
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
                if (minCandidates == 0) return 0; // dead-end cell: prune
            }
        }
    }

    return (row == -1) ? -1 : minCandidates;
}

bool OpenMPBranchBoundMRVSolver::isFeasible(const Board &board) {
    constexpr int boardSize = static_cast<int>(CommonConstants::BoardSize);

    for (int r = 0; r < boardSize; r++) {
        for (int c = 0; c < boardSize; c++) {
            if (board.getBoardBlock(r, c).getIsFilled()) continue;
            if (countCandidates(r, c, board) == 0)
                return false;
        }
    }

    return true;
}

int OpenMPBranchBoundMRVSolver::countCandidates(const int row, const int col, const Board &board) {
    int count = 0;
    for (int val = 1; val <= static_cast<int>(CommonConstants::BoardSize); val++) {
        if (isValid(row, col, val, board)) count++;
    }
    return count;
}

bool OpenMPBranchBoundMRVSolver::isValid(const int row, const int col, const int value, const Board &board) {
    constexpr int gridSize = static_cast<int>(CommonConstants::GridSize);

    const int startRow = (row / gridSize) * gridSize;
    const int startCol = (col / gridSize) * gridSize;
    const int endRow   = startRow + gridSize;
    const int endCol   = startCol + gridSize;

    return isValidInGrid(startRow, startCol, endRow, endCol, value, board) &&
           isValidInRow(row, value, board) &&
           isValidInCol(col, value, board);
}

bool OpenMPBranchBoundMRVSolver::isValidInGrid(const int startRow, const int startCol,
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

bool OpenMPBranchBoundMRVSolver::isValidInRow(const int row, const int value, const Board &board) {
    for (int c = 0; c < static_cast<int>(CommonConstants::BoardSize); c++) {
        if (board.getBoardValue(row, c) == value)
            return false;
    }
    return true;
}

bool OpenMPBranchBoundMRVSolver::isValidInCol(const int col, const int value, const Board &board) {
    for (int r = 0; r < static_cast<int>(CommonConstants::BoardSize); r++) {
        if (board.getBoardValue(r, col) == value)
            return false;
    }
    return true;
}
