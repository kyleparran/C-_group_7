

# Phase 1
- Team: 7
- Team members: Scott Turro, Kyle Parran

# todos
- Couple more questions down below (marked with todo)
- Finish and export report.xlsx into pdf

## Build instructions
cmake v3.30.0  or greater is required.

1. Build using cmake into the build folder
   ```bash
   cmake -S . -B build
   cmake --build build
   ```

2. **Run Benchmarks** by running executable `./build/MainProject.exe` or `./build/MainProject` depending on system

3. **Run Tests** by running executable `./build/tests.exe` or `./build/tests` depending on system

    - In vscode use `ctrl+shift+P` and search for `CMake: Set Launch/Debug Target` to switch targets


4. **Plot Benchmark Results** by copying and pasting the output of `MainProject` into `benchmark_results.cpp` and running `plot.py`




# Discussion Questions

- Explain the key differences between pointers and references in C++. 

    Working with pointers can make your code less clean as you need to dereference the pointers to access the data. A reference can be thought of as an alias for the original object, while a pointer is the address of the original object. This allows you to do pointer arithmetic. For example you can access the next object in memory. A pointer can also be reassigned to point to a different location, while a reference always refers to the original object.

- When would you choose to use a pointer over a reference, and vice versa, in the context of implementing numerical algorithms?

    If I need to change the object in which I'm refering to, I would prefer pointers. Otherwise I would prefer to use references as they are nicer to work with.

- How does the row-major and column-major storage order of matrices affect memory access patterns and cache locality during matrix-vector and matrix-matrix multiplication? Provide specific examples from your implementations and benchmarking results.

    When performing matrix multiplication, you need dot the top row  A[0][:] with the vector V[:]. If your matrix is in row-major form, the elements will be next to each other, leading to cache hits and speeding up your calculation. If your matrix is in column-major form, the elements of the first row will be spread out in memory, leading to cache misses and slowing down your calculation. This is why we iterator over cols then rows in the optimized version of `multiply_mv_col_major`


- Discuss the role of compiler optimizations (like inlining) in achieving high performance. How did the optimization level affect the performance of your baseline and optimized implementations? What are the potential drawbacks of aggressive optimization?

   Compiler optimizations are a significant concern if the user wants to achieve high-performance computing. When optimizations like inlining are implemented the compiler inserts the "hopefully" frequently called functions into the caller's body. I say hopefully because, inlining would not be ideal for functions that aren't frequently called, like an obscure error function. 

   In our project, increasing the compiler optimization greatly assisted our efforts in reducing the execution time. The unoptimized functions saw substantial speedups with things like better register usage and loop unrolling. The optimized functions didn't improve as significantly as the unoptimized functions due to already being mroe manually optimized, although performance overall still improved.

   Some of the drawbacks of aggressive compiler optimizations are less readable code as the code likely becomes more complex and as a result also makes it harder to debug.  

- Based on your profiling experience, what were the main performance bottlenecks in your initial implementations? How did your profiling results guide your optimization efforts?

    We had difficulty setting up a detailed profiler. Intel's vtune did not support our processors and amd's uprof kept crashing. We performed profiling with vs code's Performance Profiler which allowed use to see functions that took the most time and which lines in the function consumed the most time. However it did not give incite to cache hits, which is a major part of this assignment. Instead of using the profiler to guide our optimization we instead made small adjustments to the code (like switching to pointers or switching the order of the loops) and reran the benchmarks.

- Reflect on the teamwork aspect of this assignment. How did dividing the initial implementation tasks and then collaborating on analysis and optimization work? What were the challenges and benefits of this approach?

    A difficult part of collaborating on code is avoiding code conflicts with git. To overcome this one person started homework 1 and the other started phase 1. We then pushed our code and switched projects. This divided up tasks across both assignments and ensured no blockages. A challenge of this approach was being clear when a switch was needed (ie when someone was stuck or was about to finish their work)

