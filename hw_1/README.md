# Basic Implementation
One key reason is the overhead of repeated function calls (`add` and `getElement`). For a $4096 \times 4096$ matrix, that’s $4096^2$ function calls for both retrieving elements and adding them, which quickly adds up. Additionally, using `std::vector<std::vector<int>>` can lead to non-contiguous memory layouts, resulting in less efficient cache utilization compared to a single contiguous block of memory.

# Optimizations

- **Inlining**  
  Making `getElement` and `add` inline removes the extra cost of function calls.
  Without it, we would have $2 * 4096^2$ function calls.

- **Flat Matrix**  
  Using `std::vector<std::vector<int>>` can lead to non-contiguous memory layouts. When iterating through a matrix we would like all our values to be next to each other in memory. This increases the probability of a cache hit because the CPU loads in surrounding memory locations. Also the cpu can prefetch the data since iterating over a single vector is very predictable.

- **Pointer Arithmetic**  
  todo

- **Loop Unrolling**  
  todo

- **Other Minor Improvements We Found**
    - defining `end_ptr` instead of calculating `matrix.data() + SIZE * SIZE` in the loop
    - Using `++i`; instead of `i++`;