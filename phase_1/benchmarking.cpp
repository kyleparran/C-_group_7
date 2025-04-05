#include "benchmarking.h"
#include <cmath>
#include <random>
#include <chrono>
#include <vector>
#include <iostream>

Benchmarker::Benchmarker()
    : gen(rd()), unif(-100, 100){
};

double* Benchmarker::random_vec(int size){
    double* vec = new double[size];
    for(int i = 0; i < size; ++i){
        vec[i] = unif(gen);
    }
    return vec;
}

std::pair<double,double> Benchmarker::benchmark_mat_vec(
    void(*func)(const double*, int, int, const double*, double*),
    int rows,
    int cols,
    int runs
){

    double* matrix = random_vec(rows*cols);
    double* vec = random_vec(cols);
    double* res = new double[rows];
    double* times = new double[runs];
    std::fill(times, times + runs, 0.0);

    for(int i = 0; i < runs; i++){
        std::fill(res, res + rows, 0.0);
        
        auto start = std::chrono::high_resolution_clock::now();
        func(matrix, rows, cols, vec, res);
        auto end = std::chrono::high_resolution_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        times[i] = elapsed;
    }
    double sum = 0.0;
    for(int i = 0; i < runs; i++)
        sum += times[i];
    double mean = sum / runs;
    double var = 0.0;
    for(int i = 0; i < runs; i++)
        var += (times[i] - mean)*(times[i] - mean);
    var /= runs;
    double stddev = std::sqrt(var);

    delete[] matrix; delete[] vec; delete[] res; delete[] times;
    return {mean, stddev};
};


std::pair<double,double> Benchmarker::benchmark_mat_mat(
    void(*func)(const double*, int, int, const double*, int, int, double*),
    int dim1,
    int dim2,
    int dim3,
    int runs
){

    double* matrix1 = random_vec(dim1*dim2);
    double* matrix2 = random_vec(dim2*dim3);
    double* res = new double[dim1*dim3];
    double* times = new double[runs];
    std::fill(times, times + runs, 0.0);

    for(int i = 0; i < runs; i++){
        std::fill(res, res + dim1*dim3, 0.0);
        
        auto start = std::chrono::high_resolution_clock::now();
        func(matrix1, dim1, dim2, matrix2, dim2, dim3, res);
        auto end = std::chrono::high_resolution_clock::now();

        double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        times[i] = elapsed;
    }
    double sum = 0.0;
    for(int i = 0; i < runs; i++)
        sum += times[i];
    double mean = sum / runs;
    double var = 0.0;
    for(int i = 0; i < runs; i++)
        var += (times[i] - mean)*(times[i] - mean);
    var /= runs;
    double stddev = std::sqrt(var);

    delete[] matrix1; delete[] matrix2; delete[] res; delete[] times;
    return {mean, stddev};
};

std::pair<double,double> Benchmarker::benchmark_mat_mat_transposed(
    void(*func)(const double*, int, int, const double*, int, int, double*),
    int dim1,
    int dim2,
    int dim3,
    int runs
){
    double* matrix1 = random_vec(dim1 * dim2);
    double* matrix2 = random_vec(dim2 * dim3);
    double* transposedB = new double[dim3 * dim2];
    for(int r = 0; r < dim2; ++r){
        for(int c = 0; c < dim3; ++c){
            transposedB[c * dim2 + r] = matrix2[r * dim3 + c];
        }
    }
    double* res = new double[dim1 * dim3];
    double* times = new double[runs];
    std::fill(times, times + runs, 0.0);

    for(int i = 0; i < runs; i++){
        std::fill(res, res + dim1 * dim3, 0.0);
        auto start = std::chrono::high_resolution_clock::now();
        func(matrix1, dim1, dim2, transposedB, dim2, dim3, res);
        auto end = std::chrono::high_resolution_clock::now();
        double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        times[i] = elapsed;
    }
    double sum = 0.0;
    for(int i = 0; i < runs; i++) sum += times[i];
    double mean = sum / runs;
    double var = 0.0;
    for(int i = 0; i < runs; i++) var += (times[i] - mean) * (times[i] - mean);
    var /= runs;
    double stddev = std::sqrt(var);

    delete[] matrix1;
    delete[] matrix2;
    delete[] transposedB;
    delete[] res;
    delete[] times;
    return {mean, stddev};
}

std::string Benchmarker::get_record_line(std::string func_name, int size, int runs, double mean, double std){
    std::string line = "";
    line += func_name + ",";
    line += std::to_string(size) + ",";
    line += std::to_string(runs) + ",";
    line += std::to_string(mean) + ",";
    line += std::to_string(std);
    return line;
}