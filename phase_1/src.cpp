#include "src.h"
#include <iostream>
#include <algorithm>
#include <cmath>

void multiply_mv_row_major(const double* matrix, int rows, int cols, const double* vector, double* result){
    std::fill(result, result + cols, 0.0);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            result[i] += matrix[i*cols + j] * vector[j];
        }
    }
}

void multiply_mv_col_major(const double* matrix, int rows, int cols, const double* vector, double* result)
{
    if (!matrix || !vector || !result) return;
    if (rows <= 0 || cols <= 0) return;
    for (int i = 0; i < rows; ++i) result[i] = 0.0;
    for (int j = 0; j < cols; ++j)
    {
        for (int i = 0; i < rows; ++i)
        {
            result[i] += matrix[j * rows + i] * vector[j];
        }
    }
}

void multiply_mv_col_major_optimized(const double* matrix, int rows, int cols, const double* vector, double* result)
{
    if (!matrix || !vector || !result) return;
    if (rows <= 0 || cols <= 0) return;
    for (int i = 0; i < rows; ++i) result[i] = 0.0;
    for (int j = 0; j < cols; ++j)
    {
        double temp = vector[j];
        const double* colStart = &matrix[j * rows];
        int i = 0;
        for (; i <= rows - 4; i += 4)
        {
            result[i]   += colStart[i]   * temp;
            result[i+1] += colStart[i+1] * temp;
            result[i+2] += colStart[i+2] * temp;
            result[i+3] += colStart[i+3] * temp;
        }
        for (; i < rows; ++i)
        {
            result[i] += colStart[i] * temp;
        }
    }
}




unsigned int Factorial(unsigned int number)
{
    return number <= 1 ? number : Factorial(number - 1) * number;
}
