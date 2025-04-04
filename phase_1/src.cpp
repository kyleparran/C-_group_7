#include "src.h"
#include <iostream>
#include <algorithm>
#include <cmath>

void multiply_mv_row_major(const double* matrix, int rows, int cols, const double* vector, double* result){
    // error handling
    if (!matrix) throw std::invalid_argument("vatrix pointer is null.");
    if (!vector) throw std::invalid_argument("vector pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rows <= 0) throw std::invalid_argument("rows must be positive.");
    if (cols <= 0) throw std::invalid_argument("cols must be positive.");

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            result[i] += matrix[i*cols + j] * vector[j];
        }
    }
}

void multiply_mv_row_major_opt(const double* matrix, int rows, int cols, const double* vector, double* result){
    // error handling
    if (!matrix) throw std::invalid_argument("vatrix pointer is null.");
    if (!vector) throw std::invalid_argument("vector pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rows <= 0) throw std::invalid_argument("rows must be positive.");
    if (cols <= 0) throw std::invalid_argument("cols must be positive.");

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            result[i] += matrix[i*cols + j] * vector[j];
        }
    }
}

void multiply_mv_col_major(const double* matrix, int rows, int cols, const double* vector, double* result){
    // error handling
    if(!matrix) throw std::invalid_argument("vatrix pointer is null.");
    if(!vector) throw std::invalid_argument("vector pointer is null.");
    if(!result) throw std::invalid_argument("result pointer is null.");
    
    for (int j = 0; j < cols; ++j)
    {
        for (int i = 0; i < rows; ++i)
        {
            result[i] += matrix[j * rows + i] * vector[j];
        }
    }
}

void multiply_mv_col_major_opt(const double* matrix, int rows, int cols, const double* vector, double* result){
    // error handling
    if (!matrix) throw std::invalid_argument("vatrix pointer is null.");
    if (!vector) throw std::invalid_argument("vector pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rows <= 0) throw std::invalid_argument("rows must be positive.");
    if (cols <= 0) throw std::invalid_argument("cols must be positive.");

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
