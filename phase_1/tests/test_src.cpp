#include <catch2/catch_test_macros.hpp>
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