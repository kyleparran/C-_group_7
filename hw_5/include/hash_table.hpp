#pragma once

#include <functional>
#include <vector>

template<typename K, typename V>
class HashTable {
    using hashType = std::function<size_t(K key)>;
    using pairType = std::pair<K, V>;
    // using nodeType = std::tuple<int, K, V>;

    int currCapacity = 0;
    int maxCapacity;
    hashType _hash;
    std::vector<pairType> memoryPool;

public:

    HashTable(int maxCapacity, hashType _hash): 
        maxCapacity(maxCapacity),
        _hash(_hash), 
        memoryPool(maxCapacity)
        {}

    size_t hash(K key) {
        return _hash(key) % maxCapacity;
    }
    

    inline bool isEmpty(pairType p){
        return (p.first == K() && p.second == V());
    }

    inline int getPsl(int idx, K key){
        return (maxCapacity + idx - hash(key)) % maxCapacity;
    }

    void insert(const K& key, const V& val) {
        if (currCapacity >= maxCapacity){throw std::overflow_error("HashTable is full");}

        // Find space
        auto startIdx = hash(key);
        pairType currData(key, val);
        int currPsl = 0;  // probing sequence length
        for (int deltaIdx = 0; deltaIdx < maxCapacity; ++deltaIdx) {
            int idx = (startIdx + deltaIdx) % maxCapacity;

            // Insert if empty space
            if (isEmpty(memoryPool[idx])) {
                memoryPool[idx] = currData;
                ++currCapacity;
                return;
            }
            // Switch if it has a higher psl
            int comparePsl = getPsl(idx, memoryPool[idx].first);
            if (currPsl > comparePsl){
                std::swap(currData, memoryPool[idx]);
                std::swap(currPsl, comparePsl);
            }

            ++currPsl;
        }
        throw std::runtime_error("Could not insert into HashTable");
    }

    void remove(const K& key) {
        auto startIdx = hash(key);
        for (int deltaIdx = 0; deltaIdx < maxCapacity; ++deltaIdx) {
            int idx = (startIdx + deltaIdx) % maxCapacity;
            
            // Check if this is the key
            if (memoryPool[idx].first == key){
                // shift the rest of the chunk
                int nIdx = (idx + 1) % maxCapacity;
                int nPsl = getPsl(nIdx, memoryPool[nIdx].first);
                while (!isEmpty(memoryPool[nIdx]) && nPsl > 0){
                    memoryPool[idx] = memoryPool[nIdx];
                    idx = nIdx;
                    nIdx = (idx + 1) % maxCapacity;
                    nPsl = getPsl(nIdx, memoryPool[nIdx].first);
                }

                memoryPool[idx] = pairType();
                --currCapacity;
                return;
            }

            // Stop if empty
            else if (isEmpty(memoryPool[idx])){
                throw std::runtime_error("Could not remove from HashTable");
            }
        }
        throw std::runtime_error("Could not remove from HashTable");
    }

    V* lookup(const K& key) {
        auto startIdx = hash(key);
        for (int deltaIdx = 0; deltaIdx < maxCapacity; ++deltaIdx) {
            int idx = (startIdx + deltaIdx) % maxCapacity;
            
            // Check if this is the key
            if (memoryPool[idx].first == key){
                return &memoryPool[idx].second;
            }

            // Stop if empty
            else if (isEmpty(memoryPool[idx])){
                return nullptr;
            }
        }
        return nullptr;
    }
};