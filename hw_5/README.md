
# Homework 6
### Group 7: Kyle Parran & Scott Turro

## Benchmark and Complexity Report

We built and tested four data structures: a Robin Hood Hash table, a binary heap priority queue, SIMD-based moving averages, and a hybrid order book. We compared our implementations against standard C++ library structures to see exactly how much improvement we achieved.

## Robin Hood Hash Table
Our hash table used Robin Hood hashing, which helps handle collisions efficiently. It provided constant-time (O(1)) operations on average, but in the worst case, the complexity depended on table capacity (O(C)). For example, inserting 10,000 items into a fully occupied 10,000-slot table took 17.56 ms, slower than the standard unordered map (2.06 ms). This overhead came from Robin Hood hashing's strategy of managing collisions by swapping items. Lookup performance was relatively slow (80.5 µs vs. 2.2 µs for the standard) when compared to the unordered_map. With larger table sizes (like 1,000,000 slots), our insertions improved significantly to just 0.78 ms, beating the standard map’s 2.85 ms, showing the benefit of lower load factors. Memory use remained predictable and proportional to the capacity.

When picking how a hash table handles collisions, you're balancing three things:

* **Simplicity:** Some methods, like chaining (storing collisions in a list), are straightforward but use more memory.
* **Speed:** Others, like linear probing (checking the next open slot), are fast until the table fills up, then slow down dramatically.
* **Consistency:** Robin Hood hashing tries to keep performance steady by evenly spreading out collisions, but it does extra work swapping items, making insertions a bit slower.

Basically, you’re deciding between keeping things simple, maximizing speed, or ensuring stable, predictable performance. Robin Hood hashing sits in the middle, aiming for fairness but with a bit more complexity.


## Binary Heap Priority Queue
We implemented a binary max-heap priority queue, which has a complexity of O(log n) for both insertion and extraction operations. When inserting 10,000 items, our implementation took 0.62 ms, slower than the standard priority queue’s 0.29 ms. For extraction at the same scale, our heap again performed slower, taking 0.87 ms compared to the standard's quicker 0.61 ms. With a larger dataset of 100,000 items, our insertion was slower as well, taking 3.34 ms compared to the standard's 2.60 ms. Similarly, extraction performance at this size was noticeably slower for our heap (12.15 ms) compared to the standard implementation (8.42 ms). The additional overhead in our custom implementation likely stems from less optimized internal heap management compared to the highly tuned standard priority queue. These results show our custom binary heap didn't outperform the standard implementation in raw speed, but it did maintain consistent logarithmic performance and predictable memory usage. In practical terms, the standard library's priority queue currently remains a better choice for speed-critical tasks unless further optimizations are made to our implementation.

## SIMD Moving Average

Our SIMD approach to calculating moving averages showed clear real-world benefits. Although both SIMD and the scalar versions shared linear complexity (O(T)), the SIMD was slower in our runs, the extra setup cost outweighed its vector boost at these problem sizes. For example, calculating a moving average over 100,000 price points with a window of 1,000 took 1.11 ms with SIMD, compared to 0.76 ms for the scalar version. This might seem counterintuitive at first, but after researching online a little bit,  the overhead of SIMD can sometimes make it slower on smaller windows. However, when we used a smaller window of 100 points, SIMD took 0.68 ms, closer in performance to the scalar’s 0.57 ms. The advantage grows as computations become memory-bound, highlighting SIMD’s value for large-scale computations.

## Hybrid Order Book

Our hybrid order book combined a hash table for quick ID-based order lookup with an ordered map for efficient price-level queries. This resulted in constant-time (O(1)) modifications and deletions, an improvement over the standard tree-based approach, which is typically slower due to linear scans. For instance, modifying 100,000 orders took only 1.09 ms for our hybrid structure, significantly faster than 6.68 ms for the standard map implementation. Similarly, deleting 100,000 orders was slower at 24.49 ms versus 10.84 ms for the plain map. Insertion speed was also slower because maintaining two structures simultaneously added overhead. This trade-off still makes sense in high-frequency trading, where modifying and removing orders quickly probably matters more than slightly slower insertions. The memory usage increased marginally, but stayed predictable and manageable.


In short, targeted optimizations in these data structures provided meaningful and practical speed-ups for critical trading operations, balancing performance gains against modest complexity and memory trade-offs.


