#pragma once

#include <vector>
#include <stdexcept>

template<typename T>
class HeapPriorityQueue {
    std::vector<T> heap;

public:
    HeapPriorityQueue() = default;

    inline int getParentIdx(int idx){ return (idx - 1) / 2; }
    inline bool isEmpty() const { return heap.empty(); }
    
    void insert(const T& val) {
        heap.push_back(val);
        
        int idx = heap.size() - 1;
        while (idx > 0) {
            int pIdx = getParentIdx(idx);
            if (heap[pIdx] < heap[idx]) {
                std::swap(heap[pIdx], heap[idx]);
                idx = pIdx;
            } else {
                break;
            }
        }
    }

    T popMax() {
        if (heap.empty()){ throw std::underflow_error("HeapPriorityQueue is empty"); }

        // Replace the root of the heap with the last element on the last level.
        T maxVal = heap[0];
        heap[0] = heap.back();
        heap.pop_back();

        // Rebalance
        int idx = 0;
        while (true) {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int largest = idx;
            if (left < heap.size() && heap[largest] < heap[left]){
                largest = left;
            }
            if (right < heap.size() && heap[largest] < heap[right]){
                largest = right;
            }
            if (largest != idx) {
                std::swap(heap[idx], heap[largest]);
                idx = largest;
            } else {
                break;
            }
        }
        return maxVal;
    }

    T getMax() const {
        if (heap.empty()){ throw std::runtime_error("HeapPriorityQueue is empty");}
        return heap[0];
    }


};
