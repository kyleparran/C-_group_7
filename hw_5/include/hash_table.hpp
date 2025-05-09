#pragma once
#include <functional>
#include <vector>
#include <stdexcept>

template<typename K, typename V>
class HashTable {
    using hashType = std::function<size_t(const K&)>;
    using pairType = std::pair<K, V>;

    int currCapacity = 0;
    int maxCapacity;
    hashType _hash;
    std::vector<pairType> memoryPool;

public:
    HashTable(int maxCap, hashType h)
        : maxCapacity(maxCap), _hash(h), memoryPool(maxCap) {}

    size_t hash(const K& key) { return _hash(key) % maxCapacity; }

    inline bool isEmpty(const pairType& p) { return p.first == K(); }

    inline int getPsl(int idx, const K& key) {
        return (maxCapacity + idx - hash(key)) % maxCapacity;
    }


    void insert(const K& key, const V& val) {
        if (currCapacity >= maxCapacity){throw std::overflow_error("HashTable is full");}

        auto startIdx = hash(key);
        pairType currData(key, val);
        int currPsl = 0;  
        for (int deltaIdx = 0; deltaIdx < maxCapacity; ++deltaIdx) {
            int idx = (startIdx + deltaIdx) % maxCapacity;

            if (isEmpty(memoryPool[idx])) {
                memoryPool[idx] = currData;
                ++currCapacity;
                return;
            }
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
    int start = hash(key);
    for (int d = 0; d < maxCapacity; ++d) {
        int idx = (start + d) % maxCapacity;
        if (memoryPool[idx].first == key) {
            int next = (idx + 1) % maxCapacity;
            while (!isEmpty(memoryPool[next]) &&
                   getPsl(next, memoryPool[next].first) > 0) {
                memoryPool[idx] = memoryPool[next];
                idx = next;
                next = (idx + 1) % maxCapacity;
            }
            memoryPool[idx] = pairType();
            --currCapacity;
            return;
        }
        if (isEmpty(memoryPool[idx])) throw std::runtime_error("Key not found");
    }
    throw std::runtime_error("Key not found");
    }


    V* lookup(const K& key) {
        auto startIdx = hash(key);
        for (int deltaIdx = 0; deltaIdx < maxCapacity; ++deltaIdx) {
            int idx = (startIdx + deltaIdx) % maxCapacity;

            if (memoryPool[idx].first == key){
                return &memoryPool[idx].second;
            }

            else if (isEmpty(memoryPool[idx])){
                return nullptr;
            }
        }
        return nullptr;
    }
};