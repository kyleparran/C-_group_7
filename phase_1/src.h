#pragma once

void multiply_mv_row_major(const double* matrix, int rows, int cols, const double* vector, double* result);
void multiply_mv_row_major_opt(const double* matrix, int rows, int cols, const double* vector, double* result);

void multiply_mv_col_major(const double* matrix, int rows, int cols, const double* vector, double* result);
void multiply_mv_col_major_opt(const double* matrix, int rows, int cols, const double* vector, double* result);

void multiply_mm_naive(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result);
void multiply_mm_naive_loop(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result);
void multiply_mm_naive_unroll(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result);
void multiply_mm_naive_opt(const double* matrixA, int rowsA, int colsA, const double* matrixB, int rowsB, int colsB, double* result);

void multiply_mm_transposed_b(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB, int colsB, double* result);
void multiply_mm_transposed_b_opt(const double* matrixA, int rowsA, int colsA, const double* matrixB_transposed, int rowsB, int colsB, double* result);
