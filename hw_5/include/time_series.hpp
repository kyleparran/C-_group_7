#pragma once

#include <vector>
#include <immintrin.h>
#include <stdexcept>

inline std::vector<double> movingAverageNormal(const std::vector<double>& prices, int windowSize) {
    if (prices.size() < windowSize) { throw std::invalid_argument("Window size larger than data size"); }

    int numAverages = prices.size() - windowSize + 1;
    std::vector<double> averages(numAverages, 0.0);

    // Build up a cumSum array
    std::vector<double> cumSum(prices.size() + 1, 0.0);
    for (size_t i = 0; i < prices.size(); ++i) {
        cumSum[i + 1] = cumSum[i] + prices[i];
    }

    // Compute MA
    for (int i = 0; i < numAverages; ++i) {
        averages[i] = (cumSum[i + windowSize] - cumSum[i]) / windowSize;
    }
    return averages;
}


// todo optimize
inline std::vector<double> movingAverageSIMD(const std::vector<double>& prices, int windowSize) {
    if (prices.size() < windowSize) { throw std::invalid_argument("Window size larger than data size"); }

    int numAverages = prices.size() - windowSize + 1;
    std::vector<double> averages(numAverages, 0.0);
    __m256d divVec = _mm256_set1_pd(1.0 / windowSize);

    // Build up a cumSum array
    std::vector<double> cumSum(prices.size() + 1, 0.0);
    for (size_t i = 0; i < prices.size(); ++i) {
        cumSum[i + 1] = cumSum[i] + prices[i];
    }

    // Compute MA in chunks of 4
    size_t i = 0;
    for (; i + 3 < numAverages; i += 4) {
        __m256d p1 = _mm256_loadu_pd(&cumSum[i + windowSize]);
        __m256d p2 = _mm256_loadu_pd(&cumSum[i]);
        __m256d diff = _mm256_sub_pd(p1, p2);
        __m256d avgVec = _mm256_mul_pd(diff, divVec);
        _mm256_storeu_pd(&averages[i], avgVec);
    }
    for (; i < numAverages; ++i) {
        averages[i] = (cumSum[i + windowSize] - cumSum[i]) / windowSize;
    }
    return averages;
}