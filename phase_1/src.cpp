#include "src.h"
#include <iostream>
#include <algorithm>
#include <cmath>


inline double vector_dot(const double* vec1, const double* vec2, const double* vec2_end){
    // Performs dot product of two vectors
    double s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0,
           s5 = 0.0, s6 = 0.0, s7 = 0.0, s8 = 0.0;
    while (vec2 < vec2_end - 8) {
        s1 += vec1[0] * vec2[0];
        s2 += vec1[1] * vec2[1];
        s3 += vec1[2] * vec2[2];
        s4 += vec1[3] * vec2[3];
        s5 += vec1[4] * vec2[4];
        s6 += vec1[5] * vec2[5];
        s7 += vec1[6] * vec2[6];
        s8 += vec1[7] * vec2[7];
        vec1 += 8; vec2 += 8;
    }
    while (vec2 < vec2_end) {
        s1 += (*vec1++) * (*vec2++);
    }
    return s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8;
}

inline double vector_dot_incv2(const double* vec1, const double* vec2, const double* vec2_end,
                               int vec2_inc = 1){
    // Performs dot product of two vectors and allows for custom incrementing
    double s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0,
           s5 = 0.0, s6 = 0.0, s7 = 0.0, s8 = 0.0;
    while (vec2 < vec2_end - 8*vec2_inc) {
        s1 += vec1[0] * vec2[0];
        s2 += vec1[1] * vec2[1*vec2_inc];
        s3 += vec1[2] * vec2[2*vec2_inc];
        s4 += vec1[3] * vec2[3*vec2_inc];
        s5 += vec1[4] * vec2[4*vec2_inc];
        s6 += vec1[5] * vec2[5*vec2_inc];
        s7 += vec1[6] * vec2[6*vec2_inc];
        s8 += vec1[7] * vec2[7*vec2_inc];
        vec1 += 8; vec2 += 8*vec2_inc;
    }
    while (vec2 < vec2_end) {
        s1 += (*vec1++) * vec2[0];
        vec2 += vec2_inc;
    }
    return s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8;
}

void multiply_mv_row_major(const double* matrix, int rows, int cols, const double* vector, double* result){
    // error handling
    if (!matrix) throw std::invalid_argument("matrix pointer is null.");
    if (!vector) throw std::invalid_argument("vector pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rows <= 0) throw std::invalid_argument("rows must be positive.");
    if (cols <= 0) throw std::invalid_argument("cols must be positive.");
    std::fill(result, result + rows, 0.0);

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
    
    std::fill(result, result + rows, 0.0);

    const double* m_ptr = matrix;
    const double* v_ptr_end = vector + cols;
    for(int i = 0; i < rows; ++i){
        const double* v_ptr = vector;
        result[i] = vector_dot(m_ptr, v_ptr, v_ptr_end);
        m_ptr += cols;
    }
}

void multiply_mv_col_major(const double* matrix, int rows, int cols, const double* vector, double* result){
    // error handling
    if(!matrix) throw std::invalid_argument("matrix pointer is null.");
    if(!vector) throw std::invalid_argument("vector pointer is null.");
    if(!result) throw std::invalid_argument("result pointer is null.");
    std::fill(result, result + rows, 0.0);
    
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
    std::fill(result, result + rows, 0.0);

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

void multiply_mm_naive(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result){
    // error handling
    if (!matrixA) throw std::invalid_argument("matrixA pointer is null.");
    if (!matrixB) throw std::invalid_argument("matrixB pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rowsA <= 0) throw std::invalid_argument("rowsA must be positive.");
    if (colsA <= 0) throw std::invalid_argument("colsA must be positive.");
    if (rowsB <= 0) throw std::invalid_argument("rowsB must be positive.");
    if (colsB <= 0) throw std::invalid_argument("colsB must be positive.");
    if (colsA != rowsB) throw std::invalid_argument("dimension mismatch.");

    std::fill(result, result + (rowsA * colsB), 0.0);
    for(int i = 0; i < rowsA; ++i){
        for(int j = 0; j < colsB; ++j){
            double sum = 0.0;
            for(int k = 0; k < colsA; ++k){
                sum += matrixA[i * colsA + k] * matrixB[k * colsB + j];
            }
            result[i * colsB + j] = sum;
        }
    }
}

void multiply_mm_naive_opt(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result){
    // error handling
    if (!matrixA) throw std::invalid_argument("matrixA pointer is null.");
    if (!matrixB) throw std::invalid_argument("matrixB pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rowsA <= 0) throw std::invalid_argument("rowsA must be positive.");
    if (colsA <= 0) throw std::invalid_argument("colsA must be positive.");
    if (rowsB <= 0) throw std::invalid_argument("rowsB must be positive.");
    if (colsB <= 0) throw std::invalid_argument("colsB must be positive.");
    if (colsA != rowsB) throw std::invalid_argument("dimension mismatch.");

    std::fill(result, result + (rowsA * colsB), 0.0);

    // 153
    const double* mA_ptr_row = matrixA;
    const double* mA_ptr_end = mA_ptr_row + colsA*rowsB;
    double* r_ptr_row = result;
    while (mA_ptr_row < mA_ptr_end){
        const double* mB_ptr = matrixB;
        const double* mA_ptr_cell = mA_ptr_row;
        const double* mA_ptr_cell_end = mA_ptr_row + colsA;
        while(mA_ptr_cell < mA_ptr_cell_end){
            double* r_ptr_cell = r_ptr_row;
            const double* mB_ptr_end = mB_ptr + colsB;

            while (mB_ptr < mB_ptr_end - 4){
                r_ptr_cell[0] += mA_ptr_cell[0] * mB_ptr[0];
                r_ptr_cell[1] += mA_ptr_cell[0] * mB_ptr[1];
                r_ptr_cell[2] += mA_ptr_cell[0] * mB_ptr[2];
                r_ptr_cell[3] += mA_ptr_cell[0] * mB_ptr[3];
                mB_ptr += 4; r_ptr_cell += 4;
            }

            while (mB_ptr < mB_ptr_end){
                r_ptr_cell[0] += mA_ptr_cell[0] * mB_ptr[0];
                ++mB_ptr; ++r_ptr_cell;
            }
            ++mA_ptr_cell;
        }
        r_ptr_row += colsB;
        mA_ptr_row += colsA;
    }

    // // 170
    // double* r_ptr_row = result;
    // const double* mA_ptr = matrixA;
    // for(int i = 0; i < rowsA; ++i){
    //     const double* mB_ptr = matrixB;
    //     for(int k = 0; k < colsA; ++k){
    //         double* r_ptr_cell = r_ptr_row;
    //         const double* mB_ptr_end = mB_ptr + colsB;

    //         while (mB_ptr < mB_ptr_end - 4){
    //             r_ptr_cell[0] += mA_ptr[k] * mB_ptr[0];
    //             r_ptr_cell[1] += mA_ptr[k] * mB_ptr[1];
    //             r_ptr_cell[2] += mA_ptr[k] * mB_ptr[2];
    //             r_ptr_cell[3] += mA_ptr[k] * mB_ptr[3];
    //             mB_ptr += 4; r_ptr_cell += 4;
    //         }

    //         while (mB_ptr < mB_ptr_end){
    //             r_ptr_cell[0] += mA_ptr[k] * mB_ptr[0];
    //             ++mB_ptr; ++r_ptr_cell;
    //         }
    //     }
    //     r_ptr_row += colsB;
    //     mA_ptr += colsA;
    // }

    
    // // 190
    // const double* mA_ptr = matrixA;
    // double* r_ptr = result;
    // for(int i = 0; i < rowsA; ++i){
    //     const double* mB_ptr = matrixB;
    //     const double* mB_ptr_end = mB_ptr + rowsB * colsB;
    //     for(int j = 0; j < colsB; ++j){
    //         r_ptr[j] = vector_dot_incv2(mA_ptr, mB_ptr, mB_ptr_end, colsB);
    //         ++mB_ptr; ++mB_ptr_end;
    //     }
    //     r_ptr += colsB;
    //     mA_ptr += colsA;
    // }
}
void multiply_mm_transposed_b(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB, int colsB, double* result){
    if (!matrixA) throw std::invalid_argument("matrixA pointer is null.");
    if (!matrixB_transposed) throw std::invalid_argument("matrixB_transposed pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rowsA <= 0) throw std::invalid_argument("rowsA must be positive.");
    if (colsA <= 0) throw std::invalid_argument("colsA must be positive.");
    if (rowsB <= 0) throw std::invalid_argument("rowsB must be positive.");
    if (colsB <= 0) throw std::invalid_argument("colsB must be positive.");
    if (colsA != rowsB) throw std::invalid_argument("dimension mismatch.");
    std::fill(result, result + (rowsA * colsB), 0.0);
    for(int i = 0; i < rowsA; ++i){
        for(int j = 0; j < colsB; ++j){
            double sum = 0.0;
            for(int k = 0; k < colsA; ++k){
                sum += matrixA[i * colsA + k] * matrixB_transposed[j * rowsB + k];
            }
            result[i * colsB + j] = sum;
        }
    }
}

void multiply_mm_transposed_b_opt(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB, int colsB, double* result){
    if (!matrixA) throw std::invalid_argument("matrixA pointer is null.");
    if (!matrixB_transposed) throw std::invalid_argument("matrixB_transposed pointer is null.");
    if (!result) throw std::invalid_argument("result pointer is null.");
    if (rowsA <= 0) throw std::invalid_argument("rowsA must be positive.");
    if (colsA <= 0) throw std::invalid_argument("colsA must be positive.");
    if (rowsB <= 0) throw std::invalid_argument("rowsB must be positive.");
    if (colsB <= 0) throw std::invalid_argument("colsB must be positive.");
    if (colsA != rowsB) throw std::invalid_argument("dimension mismatch.");
    std::fill(result, result + (rowsA * colsB), 0.0);
    for(int i = 0; i < rowsA; ++i){
        for(int j = 0; j < colsB; ++j){
            double s1 = 0.0, s2 = 0.0, s3 = 0.0, s4 = 0.0;
            int k = 0;
            for(; k <= colsA - 4; k += 4){
                s1 += matrixA[i * colsA + (k + 0)] * matrixB_transposed[j * rowsB + (k + 0)];
                s2 += matrixA[i * colsA + (k + 1)] * matrixB_transposed[j * rowsB + (k + 1)];
                s3 += matrixA[i * colsA + (k + 2)] * matrixB_transposed[j * rowsB + (k + 2)];
                s4 += matrixA[i * colsA + (k + 3)] * matrixB_transposed[j * rowsB + (k + 3)];
            }
            for(; k < colsA; ++k){
                s1 += matrixA[i * colsA + k] * matrixB_transposed[j * rowsB + k];
            }
            result[i * colsB + j] = s1 + s2 + s3 + s4;
        }
    }
}