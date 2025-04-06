#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include "src.h"
#include "benchmarking.h"
#include <tuple>
#include <string>
#include <tuple>



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
    std::vector<int> sizes = {64, 128, 256, 512, 1024};
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
        
        std::tie(mean, std) = benchmarker.benchmark_mat_vec_aligned(multiply_mv_row_major, s, s, runs, 64);
        std::cout << benchmarker.get_record_line("aligned_multiply_mv_row_major", s, runs, mean, std) << std::endl;

        std::tie(mean, std) = benchmarker.benchmark_mat_vec(multiply_mv_col_major, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mv_col_major", s, runs, mean, std) << std::endl;
        
        std::tie(mean, std) = benchmarker.benchmark_mat_vec(multiply_mv_col_major_opt, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mv_col_major_opt", s, runs, mean, std) << std::endl;

        std::tie(mean, std) = benchmarker.benchmark_mat_mat(multiply_mm_naive, s, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mm_naive", s, runs, mean, std) << std::endl;
        
        std::tie(mean, std) = benchmarker.benchmark_mat_mat(multiply_mm_naive_opt, s, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mm_naive_opt", s, runs, mean, std) << std::endl;
        
        std::tie(mean, std) = benchmarker.benchmark_mat_mat_transposed(multiply_mm_transposed_b, s, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mm_transposed_b", s, runs, mean, std) << std::endl;
        
        std::tie(mean, std) = benchmarker.benchmark_mat_mat_transposed(multiply_mm_transposed_b_opt, s, s, s, runs);    
        std::cout << benchmarker.get_record_line("multiply_mm_transposed_b_opt", s, runs, mean, std) << std::endl;
    }
    
    {
        std::string alignedFile = "aligned_unaligned_results.csv";
        std::ofstream ofsAligned(alignedFile);
        ofsAligned << "function,size,runs,mean,std" << std::endl;

        std::vector<int> compareSizes = {64, 128, 256, 512, 1024};
        int runs = 5;

        for(int s : compareSizes) {
            double mean, stdv;

            // Unaligned
            std::tie(mean, stdv) = benchmarker.benchmark_mat_vec(multiply_mv_row_major, s, s, runs);
            ofsAligned << benchmarker.get_record_line("MV_RowMajor_Unaligned", s, runs, mean, stdv) << std::endl;

            // Aligned
            std::tie(mean, stdv) = benchmarker.benchmark_mat_vec_aligned(multiply_mv_row_major, s, s, runs, 64);
            ofsAligned << benchmarker.get_record_line("MV_RowMajor_Aligned", s, runs, mean, stdv) << std::endl;
        }
        
        ofsAligned.close();
        std::cout << "Wrote aligned/unaligned comparison to " << alignedFile << std::endl;
    }

    return 0;
}
