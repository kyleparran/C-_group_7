#include <iostream>
#include <chrono>
#include <random>
#include "src.h"

int main() {
    const int SIZE = 4096;
    // Random Generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> unif(-100, 100);

    // Large matrix 1
    double* matrix1 = new double[SIZE * SIZE];
    for (int i = 0; i < SIZE*SIZE; ++i) {
        matrix1[i] = unif(gen);
    }
    // Large matrix 2
    double* matrix2 = new double[SIZE * SIZE];
    for (int i = 0; i < SIZE*SIZE; ++i) {
        matrix2[i] = unif(gen);
    }
    // Large vector 1
    double* vector1 = new double[SIZE];
    for (int i = 0; i < SIZE; ++i) {
        vector1[i] = unif(gen);
    }
    // Result
    double* result = new double[SIZE];

    auto start = std::chrono::high_resolution_clock::now();
    multiply_mv_row_major(matrix1, SIZE, SIZE, vector1, result);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Time: " << duration.count() << " milliseconds" << std::endl;

    //todo free vars?
    return 0;
}