#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <cmath>
#include <algorithm>
#include "src.h"
#include "benchmarking.h"




int main() {
    Benchmarker benchmarker;

    // Run specific function
    const int size = 500;
    double* matrix1 = benchmarker.random_vec(size*size);
    double* matrix2 = benchmarker.random_vec(size*size);
    double* vec = benchmarker.random_vec(size);
    double* res = new double[size * size];

    auto start = std::chrono::high_resolution_clock::now();
    //multiply_mv_row_major_opt(matrix1, size, size, vec, res);
    multiply_mm_naive(matrix1, size, size, matrix2, size, size, res);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Time: " << duration.count() << " milliseconds" << std::endl;
    delete[] matrix1; delete[] matrix2; delete[] vec; delete[] res;


    // benchmark
    // std::vector<int> sizes = {256, 512, 1024, 2048};
    std::vector<int> sizes = {64, 128, 256, 512};
    int runs = 5;
    std::cout << "Benchmarks" << std::endl;
    std::cout << "----------" << std::endl;
    std::cout << "function,size,runs,mean,std" << std::endl;
    for(int s : sizes){
        double mean, std;

        std::tie(mean, std) = benchmarker.benchmark_mat_vec(multiply_mv_row_major, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mv_row_major", s, runs, mean, std) << std::endl;
        
        std::tie(mean, std) = benchmarker.benchmark_mat_vec(multiply_mv_row_major_opt, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mv_row_major_opt", s, runs, mean, std) << std::endl;
        
        std::tie(mean, std) = benchmarker.benchmark_mat_vec(multiply_mv_col_major, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mv_col_major", s, runs, mean, std) << std::endl;
        
        std::tie(mean, std) = benchmarker.benchmark_mat_vec(multiply_mv_col_major_opt, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mv_col_major_opt", s, runs, mean, std) << std::endl;

        std::tie(mean, std) = benchmarker.benchmark_mat_mat(multiply_mm_naive, s, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mm_naive", s, runs, mean, std) << std::endl;
        
        std::tie(mean, std) = benchmarker.benchmark_mat_mat(multiply_mm_naive_opt, s, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mm_naive_opt", s, runs, mean, std) << std::endl;
        
        // std::tie(mean, std) = benchmarker.benchmark_mat_mat(multiply_mm_transposed_b, s, s, s, runs);    
        // std::cout << benchmarker.get_record_line("multiply_mm_transposed_b", s, runs, mean, std) << std::endl;
        
        // std::tie(mean, std) = benchmarker.benchmark_mat_mat(multiply_mm_transposed_b_opt, s, s, s, runs);    
        // std::cout << benchmarker.get_record_line("multiply_mm_transposed_b_opt", s, runs, mean, std) << std::endl;
        
    }

    return 0;
}