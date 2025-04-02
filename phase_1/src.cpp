#include "src.h"
#include <iostream>

void multiply_mv_row_major(const double* matrix, int rows, int cols, const double* vector, double* result){
    std::fill(result, result + cols, 0.0);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < cols; ++j){
            result[i] += matrix[i*cols + j] * vector[j];
        }
    }
}


unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}