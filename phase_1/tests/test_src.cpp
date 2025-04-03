#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>
#include <cmath>
#include <random>
#include <chrono>
#include <vector>
#include <iostream>
#include "src.h"

inline bool approx_equal_dbl(double a, double b, double epsilon = 1e-10) {
    return std::abs(a - b) < epsilon;
}


TEST_CASE("2x3 matrix", "[multiply_mv_row_major]") {
    double matrix[] = {1.0, 2.0, 3.0, 
                       4.0, 5.0, 6.0};
    double vector[] = {1.0, 2.0, 3.0};
    double* result = new double[2];

    multiply_mv_row_major(matrix, 2, 3, vector, result);
    
    REQUIRE(approx_equal_dbl(result[0], 14.0));
    REQUIRE(approx_equal_dbl(result[1], 32.0));

    delete[] result;
}

TEST_CASE("2x3 (column-major) unoptimized", "[multiply_mv_col_major]")
{
    double matrix_col[] = {1.0,4.0, 2.0,5.0, 3.0,6.0};
    double vector[] = {1.0,2.0,3.0};
    double* result = new double[2];
    multiply_mv_col_major(matrix_col, 2, 3, vector, result);
    REQUIRE(approx_equal_dbl(result[0], 14.0));
    REQUIRE(approx_equal_dbl(result[1], 32.0));
    delete[] result;
}

TEST_CASE("2x3 (column-major) optimized", "[multiply_mv_col_major_optimized]")
{
    double matrix_col[] = {1.0,4.0, 2.0,5.0, 3.0,6.0};
    double vector[] = {1.0,2.0,3.0};
    double* result = new double[2];
    multiply_mv_col_major_optimized(matrix_col, 2, 3, vector, result);
    REQUIRE(approx_equal_dbl(result[0], 14.0));
    REQUIRE(approx_equal_dbl(result[1], 32.0));
    delete[] result;
}

static std::pair<double,double> benchmark_col_major(
    void(*func)(const double*,int,int,const double*,double*),
    int rows,
    int cols,
    int runs
){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-100.0, 100.0);
    std::vector<double> matrix(rows*cols);
    std::vector<double> vec(cols);
    std::vector<double> res(rows);
    std::vector<double> times;
    times.reserve(runs);
    for(int i = 0; i < rows*cols; i++) matrix[i] = dist(gen);
    for(int i = 0; i < cols; i++) vec[i] = dist(gen);
    for(int r = 0; r < runs; r++)
    {
        std::fill(res.begin(), res.end(), 0.0);
        auto start = std::chrono::high_resolution_clock::now();
        func(matrix.data(), rows, cols, vec.data(), res.data());
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        times.push_back(elapsed);
    }
    double sum = 0.0;
    for(double t : times) sum += t;
    double mean = sum / runs;
    double var = 0.0;
    for(double t : times) var += (t - mean)*(t - mean);
    var /= runs;
    double stddev = std::sqrt(var);
    return {mean,stddev};
}

TEST_CASE("Benchmark column-major functions", "[benchmark]")
{
    std::vector<int> sizes = {256, 512, 1024};
    int runs = 5;
    std::cout << "\nColumn-Major Benchmark (microseconds)\n";
    std::cout << "Runs: " << runs << "\n";
    std::cout << "Size\tUnopt (mean,stddev)\tOpt (mean,stddev)\n";
    for(int s : sizes)
    {
        auto [meanU,stdU] = benchmark_col_major(multiply_mv_col_major, s, s, runs);
        auto [meanO,stdO] = benchmark_col_major(multiply_mv_col_major_optimized, s, s, runs);
        std::cout << s << "x" << s << "\t("
                  << meanU << "," << stdU << ")\t("
                  << meanO << "," << stdO << ")\n";
    }
}