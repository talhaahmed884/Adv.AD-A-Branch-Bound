cmake_minimum_required(VERSION 3.15)
project(Adv_AD_A_Branch_Bound)

set(CMAKE_CXX_STANDARD 17)

add_executable(Adv_AD_A_Branch_Bound main.cpp
        SudokuBoard/Block/Block.cpp
        SudokuBoard/Block/Block.h
        SudokuBoard/Board/Board.cpp
        SudokuBoard/Board/Board.h
        BoardGenerator/BoardGenerator.cpp
        BoardGenerator/BoardGenerator.h
        SudokuBoard/CommonConstants.h
        BoardGenerator/PuzzleBank.h
        BoardSolver/Solver.h
        BoardSolver/SerialSolver/SerialSolver.cpp
        BoardSolver/SerialSolver/SerialSolver.h
        BoardSolver/SerialMRVSolver/SerialMRVSolver.cpp
        BoardSolver/SerialMRVSolver/SerialMRVSolver.h
        BoardSolver/OpenMPSolver/OpenMPSolver.cpp
        BoardSolver/OpenMPSolver/OpenMPSolver.h
        BoardSolver/OpenMPBranchBoundMRVSolver/OpenMPBranchBoundMRVSolver.cpp
        BoardSolver/OpenMPBranchBoundMRVSolver/OpenMPBranchBoundMRVSolver.h
        BoardSolver/BranchBoundSolver/BranchBoundSolver.cpp
        BoardSolver/BranchBoundSolver/BranchBoundSolver.h
        BoardSolver/BranchBoundMRVSolver/BranchBoundMRVSolver.cpp
        BoardSolver/BranchBoundMRVSolver/BranchBoundMRVSolver.h
        CorrectnessChecker/CorrectnessChecker.cpp
        CorrectnessChecker/CorrectnessChecker.h
        Benchmark/BenchmarkRunner.cpp
        Benchmark/BenchmarkRunner.h
        Benchmark/PuzzleProfiler/PuzzleProfiler.h
        Benchmark/PuzzleProfiler/PuzzleProfiler.cpp
)

target_compile_definitions(Adv_AD_A_Branch_Bound PRIVATE "PROJECT_ROOT=\"${CMAKE_SOURCE_DIR}\"")

option(HPC "Build with manual -fopenmp flags for HPC clusters" OFF)

if (HPC)
    message(STATUS "HPC mode: applying -fopenmp flags directly")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")
elseif (APPLE)
    set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/libomp;/usr/local/opt/libomp" ${CMAKE_PREFIX_PATH})
    find_package(OpenMP)
    if (OpenMP_CXX_FOUND)
        target_link_libraries(Adv_AD_A_Branch_Bound PRIVATE OpenMP::OpenMP_CXX)
    else ()
        message(WARNING "OpenMP not found; the solver will build but run serially.")
    endif ()
else ()
    find_package(OpenMP)
    if (OpenMP_CXX_FOUND)
        target_link_libraries(Adv_AD_A_Branch_Bound PRIVATE OpenMP::OpenMP_CXX)
    else ()
        message(WARNING "OpenMP not found; the solver will build but run serially.")
    endif ()
endif ()
