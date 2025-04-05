# Basic Implementation
One key reason is the overhead of repeated function calls (`add` and `getElement`). For a $4096 \times 4096$ matrix, that’s $4096^2$ function calls for both retrieving elements and adding them, which quickly adds up. Additionally, using `std::vector<std::vector<int>>` can lead to non-contiguous memory layouts, resulting in less efficient cache utilization compared to a single contiguous block of memory.

# Target/Execution
We initially set a target execution time of under 50 milliseconds as the intial execution time was approximately 200 milliseconds on our hardware. We surpassed this target significantly, with runs averaging around 12 milliseconds post-optimization.

# Optimizations

- **Inlining**  
  Making `getElement` and `add` inline removes the extra cost of function calls.
  Without it, we would have $2 * 4096^2$ function calls.

- **Flat Matrix**  
  Using `std::vector<std::vector<int>>` can lead to non-contiguous memory layouts. When iterating through a matrix we would like all our values to be next to each other in memory. This increases the probability of a cache hit because the CPU loads in surrounding memory locations. Also the cpu can prefetch the data since iterating over a single vector is very predictable.

- **Pointer Arithmetic**  
  Everytime we call `matrix[i]` the code calls the [] function of the vector with a value of 1. This function counts i steps from the starting address of the vector. By using moving a pointer along the matrix, we can traverse the array without having to recompute the desired location. We also eliminated the function call.

- **Loop Unrolling**  
  Modern Processors can process multiple lines at the same time. By unrolling our loop we can process chunks of the matrix at the same time.

- **Other Minor Improvements We Found**
    - defining `end_ptr` instead of calculating `matrix.data() + SIZE * SIZE` in the loop
    - Using `++i`; instead of `i++`;