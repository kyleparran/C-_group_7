#include <iostream>
#include <vector>
#include <chrono>
#include <random>

const int SIZE = 4096;

inline int getElement(const std::vector<std::vector<int>>& matrix, int row, int col) {
    return matrix[row][col];
}

inline int add(int a, int b) {
    return a + b;
}

long long sumMatrixBasic(const std::vector<std::vector<int>>& matrix) {
    long long sum = 0;
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            sum = add(sum, getElement(matrix, i, j));
        }
    }
    return sum;
}

inline long long sumMatrixOptimized(const std::vector<int>& matrix) {
    const int* ptr = matrix.data();
    const int* end_ptr = ptr + SIZE * SIZE;
    long long s1 = 0, s2 = 0, s3 = 0, s4 = 0;
    while (ptr <= end_ptr - 4) {
        s1 += ptr[0];
        s2 += ptr[1];
        s3 += ptr[2];
        s4 += ptr[3];
        ptr += 4;
    }
    while (ptr < end_ptr) {
        s1 += *ptr++;
    }
    return s1 + s2 + s3 + s4;
}

int main() {
    std::vector<std::vector<int>> matrix(SIZE, std::vector<int>(SIZE));
    std::vector<int> flat_matrix(SIZE * SIZE);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-100, 100);
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            int value = distrib(gen);
            matrix[i][j] = value;
            flat_matrix[i * SIZE + j] = value;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();
    long long sum = sumMatrixBasic(matrix);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Basic Sum: " << sum << std::endl;
    std::cout << "Basic Time: " << duration.count() << " milliseconds" << std::endl;

    auto start_optimized = std::chrono::high_resolution_clock::now();
    long long optimized_sum = sumMatrixOptimized(flat_matrix);
    auto end_optimized = std::chrono::high_resolution_clock::now();
    auto duration_optimized = std::chrono::duration_cast<std::chrono::milliseconds>(end_optimized - start_optimized);

    std::cout << "Optimized Sum: " << optimized_sum << std::endl;
    std::cout << "Optimized Time: " << duration_optimized.count() << " milliseconds" << std::endl;

    return 0;
}
