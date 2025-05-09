#pragma once
#include <vector>
#include <stdexcept>
#include <emmintrin.h>                   


inline std::vector<double>
movingAverageNormal(const std::vector<double>& prices, int windowSize)
{
    if (prices.size() < static_cast<size_t>(windowSize))
        throw std::invalid_argument("Window size larger than data size");

    int n = static_cast<int>(prices.size()) - windowSize + 1;
    std::vector<double> avg(n);

    std::vector<double> c(prices.size() + 1, 0.0);
    for (size_t i = 0; i < prices.size(); ++i) c[i + 1] = c[i] + prices[i];

    for (int i = 0; i < n; ++i)
        avg[i] = (c[i + windowSize] - c[i]) / windowSize;

    return avg;
}

inline std::vector<double>
movingAverageSIMD(const std::vector<double>& prices, int windowSize)
{
    if (prices.size() < static_cast<size_t>(windowSize))
        throw std::invalid_argument("Window size larger than data size");

    int n = static_cast<int>(prices.size()) - windowSize + 1;
    std::vector<double> avg(n);

    std::vector<double> c(prices.size() + 1, 0.0);
    for (size_t i = 0; i < prices.size(); ++i) c[i + 1] = c[i] + prices[i];

    __m128d div = _mm_set1_pd(1.0 / windowSize);

    int i = 0;
    for (; i + 1 < n; i += 2) {
        __m128d hi = _mm_loadu_pd(&c[i + windowSize]); 
        __m128d lo = _mm_loadu_pd(&c[i]);              
        __m128d diff = _mm_sub_pd(hi, lo);             
        __m128d res  = _mm_mul_pd(diff, div);          
        _mm_storeu_pd(&avg[i], res);                   
    }
    for (; i < n; ++i)
        avg[i] = (c[i + windowSize] - c[i]) / windowSize;

    return avg;
}
