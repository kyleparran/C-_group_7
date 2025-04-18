# HW 3: Group 7
Kyle Parran and Scott Turro

# Part 1 Answers
What happens if you use delete on the array?
Using delete instead of delete[] for an array causes undefined behavior.
In practice, it typically calls only the destructor of the first element, leaving other objects destructors uncalled and causing memory leaks or heap corruption. Using delete[] correctly calls the destructor for each element in the array.
It then releases the entire block of memory back to the heap.

What happens if you forget to call delete at all?
Forgetting to call delete creates a memory leak. The allocated memory stays reserved throughout the program’s lifetime, potentially causing the program to consume more and more memory, eventually exhausting system resources.

Can you call delete twice on the same pointer?
No, calling delete twice is undefined behavior. When I do call it i get an "exception" error, not allowing the program to run.

# Part 2 Answers:
In - code

# Part 3 Answers:
Why do we delete the copy constructor?
The copy constructor is deleted to ensure that two instances of TradeHandle don't hold the same raw pointer at the same time. If two handles own the same resource, when one goes out of scope, the other would reference already deleted memory , as we learned it is a dangling pointer. This could cause crashes or corruption.

Why is move semantics allowed?
Move semantics let you safely transfer ownership of a resource from one object to another. It ensures that at any given time, exactly one object clearly owns the resource, preventing issues like accidentally deleting something twice or creating unwanted copies.

What happens if you don't define a destructor?
If you don't define a destructor, the dynamically allocated object won't ever get deleted, causing it to remain in memory permanently. Over time, these objects build up, consuming more and more memory. Eventually, this can cause performance issues or even make your system run out of available memory.


# Part 5 Answers:

What are the 3 biggest risks of manual memory management?
- Memory leaks: Unfreed memory can accumulate, leading to increased memory usage and potential application crashes.
- Dangling pointers: Accessing memory after it has been freed can lead to undefined behavior, crashes, or data corruption.
- Fragmentation: Inefficient use of memory can lead to increased fragmentation, reducing performance and available memory.

What real-world problems could occur in HFT systems due to memory leaks or fragmentation?
Memory leaks and fragmentation can increase latency, slowing down processing times and affecting trade execution speed. Memory issues can lead to system crashes and unexpected shutdowns during trading hours. 

How does RAII reduce the risk of bugs?
It ensures that data is properly allocated and deallocated. It does this by providing an interface to interact with the objects. This wrapper handles the clean up of the data, reducing the probability that coder messes it up.

What tradeoffs exist between manual and automatic memory management in performance-critical code?
Manual memory management offers more control and can be optimized for specific use cases. However, it increases the risk of memory-related bugs.
- RAII takes responsibility of managing memory decreasing the risk of errors. The trade off is the inability to optimize performance.