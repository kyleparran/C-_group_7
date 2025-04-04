#include "src.h"
#include <iostream>
#include <algorithm>
#include <cmath>

void multiply_mv_row_major(const double* matrix, int rows, int cols, const double* vector, double* result){
    // error handling
    if (!matrix) throw std::invalid_argument("matrix pointer is null.");
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
    if (!matrix) throw std::invalid_argument("matrix pointer is null.");
    if (!vector) throw std::invalid_argument("vector pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rows <= 0) throw std::invalid_argument("rows must be positive.");
    if (cols <= 0) throw std::invalid_argument("cols must be positive.");

    const double* m_ptr = matrix;
    const double* v_ptr_end = vector + cols;
    for(int i = 0; i < rows; ++i){
        double s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
        const double* v_ptr = vector;
        while (v_ptr <= v_ptr_end - 4) {
            s1 += m_ptr[0] * v_ptr[0];
            s2 += m_ptr[1] * v_ptr[1];
            s3 += m_ptr[2] * v_ptr[2];
            s4 += m_ptr[3] * v_ptr[3];
            m_ptr += 4; v_ptr += 4;
        }
        while (v_ptr < v_ptr_end) {
            s1 += (*m_ptr++) * (*v_ptr++);
        }
        result[i] = s1 + s2 + s3 + s4;
    }
}

void multiply_mv_col_major(const double* matrix, int rows, int cols, const double* vector, double* result){
    // error handling
    if(!matrix) throw std::invalid_argument("matrix pointer is null.");
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
    if (!matrix) throw std::invalid_argument("matrix pointer is null.");
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
