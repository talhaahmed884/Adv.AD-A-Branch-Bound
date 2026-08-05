#ifndef ADV_AD_A_BRANCH_BOUND_BENCHMARKRUNNER_H
#define ADV_AD_A_BRANCH_BOUND_BENCHMARKRUNNER_H
#pragma once

#include <string>
#include <vector>

#include "../SudokuBoard/Board/Board.h"
using namespace std;

struct BenchmarkResult {
    string boardId;
    string difficulty;
    string algorithm;
    int threads;
    int rep;
    double timeSec;
    int correct;
};

class BenchmarkRunner {
public:
    static void run(const string &outputDir, int puzzleCount = 20);

private:
    static void runBenchmarks(const string &outputDir, int puzzleCount);

    static vector<BenchmarkResult> benchmarkSerial(const Board &board, const string &boardId,
                                                   const string &difficulty);

    static vector<BenchmarkResult> benchmarkSerialMRV(const Board &board, const string &boardId,
                                                      const string &difficulty);

    static vector<BenchmarkResult> benchmarkOpenMP(const Board &board, const string &boardId,
                                                   const string &difficulty, int threads);

    static void writeCsv(const vector<BenchmarkResult> &results, const string &outputPath);

    static string makeOutputPath(const string &outputDir, const string &prefix);

    static void printRow(const BenchmarkResult &r);
};

#endif //ADV_AD_A_BRANCH_BOUND_BENCHMARKRUNNER_H
