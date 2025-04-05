#pragma once

#include <iostream>
#include <vector>
#include <random>


class Benchmarker {
    public:
        Benchmarker();
        
        double* random_vec(int size);
        std::pair<double,double> benchmark_mat_vec(
            void(*func)(const double*, int, int, const double*, double*),
            int rows,
            int cols,
            int runs
        );
        std::pair<double,double> benchmark_mat_mat(
            void(*func)(const double*, int, int, const double*, int, int, double*),
            int dim1,
            int dim2,
            int dim3,
            int runs
        );
        std::pair<double,double> benchmark_mat_mat_transposed(
            void(*func)(const double*, int, int, const double*, int, int, double*),
            int dim1,
            int dim2,
            int dim3,
            int runs
        );
        std::string get_record_line(std::string func_name, int size, int runs, double mean, double std);

    private:
        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<double> unif;
};