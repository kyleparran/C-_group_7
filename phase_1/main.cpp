/***** main.cpp (Benchmark and Basic Tests) *****/
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <cmath>
#include <algorithm>
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

    delete[] matrix1;
    delete[] matrix2;
    delete[] vector1;
    delete[] result;
    
    static auto benchmark_function = [](void (*func)(const double*, int, int, const double*, double*),
    int rows, int cols, int runs)
{
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> dist(-100.0, 100.0);
std::vector<double> matrix(rows*cols);
std::vector<double> vec(cols);
std::vector<double> res(rows);
std::vector<double> times;
times.reserve(runs);
for(int i = 0; i < rows*cols; i++) matrix[i] = dist(gen);
for(int i = 0; i < cols; i++) vec[i] = dist(gen);
for(int r = 0; r < runs; r++){
std::fill(res.begin(), res.end(), 0.0);
auto start = std::chrono::high_resolution_clock::now();
func(matrix.data(), rows, cols, vec.data(), res.data());
auto end = std::chrono::high_resolution_clock::now();
double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
times.push_back(elapsed);
}
double sum = 0.0;
for(double t : times) sum += t;
double mean = sum / runs;
double var = 0.0;
for(double t : times) var += (t - mean)*(t - mean);
var /= runs;
double stddev = std::sqrt(var);
return std::make_pair(mean, stddev);
};

{
int rows = 2;
int cols = 3;
double* matrix = new double[rows * cols];
double* vector = new double[cols];
double* res = new double[rows];
matrix[0] = 1.0; matrix[1] = 2.0; matrix[2] = 3.0;
matrix[3] = 4.0; matrix[4] = 5.0; matrix[5] = 6.0;
for(int i = 0; i < cols; ++i) vector[i] = i + 1.0;
multiply_mv_row_major(matrix, rows, cols, vector, res);
bool rowTest = std::fabs(res[0] - 14.0) < 1e-10 && std::fabs(res[1] - 32.0) < 1e-10;
if(rowTest) std::cout << "Row-major 2x3 test passed\n"; else std::cout << "Row-major 2x3 test failed\n";
delete[] matrix;
delete[] vector;
delete[] res;
}
{
int rows = 2;
int cols = 3;
double* matrix = new double[rows * cols];
double* vector = new double[cols];
double* res = new double[rows];
matrix[0] = 1.0; matrix[1] = 4.0; matrix[2] = 2.0;
matrix[3] = 5.0; matrix[4] = 3.0; matrix[5] = 6.0;
for(int i = 0; i < cols; ++i) vector[i] = i + 1.0;
multiply_mv_col_major(matrix, rows, cols, vector, res);
bool test1 = std::fabs(res[0] - 14.0) < 1e-10 && std::fabs(res[1] - 32.0) < 1e-10;
std::fill(res, res + rows, 0.0);
multiply_mv_col_major_optimized(matrix, rows, cols, vector, res);
bool test2 = std::fabs(res[0] - 14.0) < 1e-10 && std::fabs(res[1] - 32.0) < 1e-10;
if(test1) std::cout << "Col-major 2x3 unoptimized passed\n"; else std::cout << "Col-major 2x3 unoptimized failed\n";
if(test2) std::cout << "Col-major 2x3 optimized passed\n"; else std::cout << "Col-major 2x3 optimized failed\n";
delete[] matrix;
delete[] vector;
delete[] res;
}

std::vector<int> sizes = {256, 512, 1024};
int runs = 5;
std::cout << "\nRow-Major vs Column-Major Benchmarks\n";
std::cout << "Matrix Size   RowMajor(us)       ColMajorUnopt(us)     ColMajorOpt(us)\n";
for(int s : sizes){
auto [meanR, stdR] = benchmark_function(multiply_mv_row_major, s, s, runs);
auto [meanU, stdU] = benchmark_function(multiply_mv_col_major, s, s, runs);
auto [meanO, stdO] = benchmark_function(multiply_mv_col_major_optimized, s, s, runs);
std::cout << s << "x" << s << "       "
<< meanR << "           "
<< meanU << "              "
<< meanO << "\n";
}
return 0;
}