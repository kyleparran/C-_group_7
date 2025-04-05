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


TEST_CASE("2x3, row-major, unoptimized", "[multiply_mv_row_major]") {
    double matrix[] = {1.0, 2.0, 3.0, 
                       4.0, 5.0, 6.0};
    double vector[] = {1.0, 2.0, 3.0};
    double* res = new double[2];

    multiply_mv_row_major(matrix, 2, 3, vector, res);
    REQUIRE(approx_equal_dbl(res[0], 14.0));
    REQUIRE(approx_equal_dbl(res[1], 32.0));
    delete[] res;
}

TEST_CASE("2x3, row-major, optimized", "[multiply_mv_row_major_opt]") {
    double matrix[] = {1.0, 2.0, 3.0, 
                       4.0, 5.0, 6.0};
    double vector[] = {1.0, 2.0, 3.0};
    double* res = new double[2];

    multiply_mv_row_major_opt(matrix, 2, 3, vector, res);
    REQUIRE(approx_equal_dbl(res[0], 14.0));
    REQUIRE(approx_equal_dbl(res[1], 32.0));
    delete[] res;
}

TEST_CASE("2x3, column-major, unoptimized", "[multiply_mv_col_major]")
{
    double matrix_col[] = {1.0, 4.0, 
                           2.0, 5.0, 
                           3.0, 6.0};
    double vector[] = {1.0, 2.0, 3.0};
    double* res = new double[2];

    multiply_mv_col_major(matrix_col, 2, 3, vector, res);
    REQUIRE(approx_equal_dbl(res[0], 14.0));
    REQUIRE(approx_equal_dbl(res[1], 32.0));
    delete[] res;
}

TEST_CASE("2x3, column-major, optimized", "[multiply_mv_col_major_opt]")
{
    double matrix_col[] = {1.0, 4.0, 
                           2.0, 5.0, 
                           3.0, 6.0};
    double vector[] = {1.0, 2.0, 3.0};
    double* res = new double[2];

    multiply_mv_col_major_opt(matrix_col, 2, 3, vector, res);
    REQUIRE(approx_equal_dbl(res[0], 14.0));
    REQUIRE(approx_equal_dbl(res[1], 32.0));
    delete[] res;
}


TEST_CASE("2x3 x 3x2, naive", "[multiply_mm_naive]")
{
    double matrixA[] = {
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0
    };
    double matrixB[] = {
        7.0,  8.0,
        9.0, 10.0,
        11.0, 12.0
    };
    double* res = new double[2 * 2];

    multiply_mm_naive(matrixA, 2, 3, matrixB, 3, 2, res);

    REQUIRE(approx_equal_dbl(res[0],  58.0));
    REQUIRE(approx_equal_dbl(res[1],  64.0));
    REQUIRE(approx_equal_dbl(res[2], 139.0));
    REQUIRE(approx_equal_dbl(res[3], 154.0));

    delete[] res;
}

TEST_CASE("2x3 x 3x2, naive_opt", "[multiply_mm_naive_opt]")
{
    double matrixA[] = {
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0
    };
    double matrixB[] = {
        7.0,  8.0,
        9.0, 10.0,
        11.0, 12.0
    };
    double* res = new double[2 * 2];

    multiply_mm_naive_opt(matrixA, 2, 3, matrixB, 3, 2, res);

    REQUIRE(approx_equal_dbl(res[0],  58.0));
    REQUIRE(approx_equal_dbl(res[1],  64.0));
    REQUIRE(approx_equal_dbl(res[2], 139.0));
    REQUIRE(approx_equal_dbl(res[3], 154.0));

    delete[] res;
}

TEST_CASE("2x3 x 3x2, transposed B", "[multiply_mm_transposed_b]")
{
    double matrixA[] = {
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0
    };
    double matrixB_t[] = {
        7.0,  9.0,  11.0,
        8.0, 10.0,  12.0
    };
    double* res = new double[2 * 2];
    multiply_mm_transposed_b(matrixA, 2, 3, matrixB_t, 3, 2, res);
    REQUIRE(approx_equal_dbl(res[0], 58.0));
    REQUIRE(approx_equal_dbl(res[1], 64.0));
    REQUIRE(approx_equal_dbl(res[2], 139.0));
    REQUIRE(approx_equal_dbl(res[3], 154.0));
    delete[] res;
}

TEST_CASE("2x3 x 3x2, transposed B opt", "[multiply_mm_transposed_b_opt]")
{
    double matrixA[] = {
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0
    };
    double matrixB_t[] = {
        7.0,  9.0,  11.0,
        8.0, 10.0,  12.0
    };
    double* res = new double[2 * 2];
    multiply_mm_transposed_b_opt(matrixA, 2, 3, matrixB_t, 3, 2, res);
    REQUIRE(approx_equal_dbl(res[0], 58.0));
    REQUIRE(approx_equal_dbl(res[1], 64.0));
    REQUIRE(approx_equal_dbl(res[2], 139.0));
    REQUIRE(approx_equal_dbl(res[3], 154.0));
    delete[] res;
}

        

