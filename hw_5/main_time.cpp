#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include "time_series.hpp"

void testMovingAverages() {
    std::vector<double> prices = {100.0, 102.0, 101.0, 105.0, 110.0, 108.0, 107.0, 111.0};
    int windowSize = 3;

    // Compute scalar moving average
    auto averagesScalar = movingAverageNormal(prices, windowSize);
    auto averagesSIMD = movingAverageSIMD(prices, windowSize);

    bool allEqual = true;
    for (size_t i = 0; i < averagesScalar.size(); ++i) {
        if (abs(averagesScalar[i] - averagesSIMD[i]) > 1e-6) {
            allEqual = false;
            break;
        }
    }

    if (allEqual) {
        std::cout << "TEST PASSED: RESULTS EQUAL\n";
    } else {
        std::cout << "TEST FAILED: RESULTS NOT EQUAL\n";
        std::cout << "\tNormal Moving Averages:\n\t\t";
        for (const auto& val : averagesScalar) {
            std::cout << val << " ";
        }
        std::cout << "\n";

        std::cout << "\tSIMD Moving Averages:\n\t\t";
        for (const auto& val : averagesSIMD) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    std::cout <<"\n";
}

void benchmarkMovingAverage(int numDataPoints, int windowSize) {
    std::cout << "Benchmarking with " << numDataPoints << " data points and window size " << windowSize << "\n";

    // Print headers
    std::cout << "operation,SIMD,Normal,diff\n";

    // Create prices
    std::vector<double> prices(numDataPoints);
    for (int i = 0; i < numDataPoints; ++i) {
        prices[i] = 100.0 + (i % 100) * 0.01;
    }

    // Benchmark Moving Average
    auto start = std::chrono::high_resolution_clock::now();
    auto averagesScalar = movingAverageSIMD(prices, windowSize);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed1 = end - start;
    // Comapre to Normal Method
    start = std::chrono::high_resolution_clock::now();
    auto averagesSIMD = movingAverageNormal(prices, windowSize);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed2 = end - start;
    // Print results
    std::cout << "movingAverage," << 
                 elapsed1.count() << "," <<
                 elapsed2.count() << "," << 
                 elapsed1.count() - elapsed2.count() << "\n";

    std::cout <<"\n";
}

int main() {
    testMovingAverages();
    benchmarkMovingAverage(10000, 100);
    benchmarkMovingAverage(10000, 1000);
    benchmarkMovingAverage(100000, 100);
    benchmarkMovingAverage(100000, 1000);
    return 0;
}
