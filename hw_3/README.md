HW 3: Group 7
Kyle Parran and Scott Turro

Part 1 Answers
What happens if you use delete on the array?
Using delete instead of delete[] for an array causes undefined behavior.
In practice, it typically calls only the destructor of the first element, leaving other objects destructors uncalled and causing memory leaks or heap corruption. Using delete[] correctly calls the destructor for each element in the array.
It then releases the entire block of memory back to the heap.

What happens if you forget to call delete at all?
Forgetting to call delete creates a memory leak. The allocated memory stays reserved throughout the program’s lifetime, potentially causing the program to consume more and more memory, eventually exhausting system resources.

Can you call delete twice on the same pointer?
No, calling delete twice is undefined behavior. When I do call it i get an "exception" error, not allowing the program to run.

Part 2 Answers:
In - code

Part 3 Answers:
Why do we delete the copy constructor?
The copy constructor is deleted to ensure that two instances of TradeHandle don't hold the same raw pointer at the same time. If two handles own the same resource, when one goes out of scope, the other would reference already deleted memory , as we learned it is a dangling pointer. This could cause crashes or corruption.

Why is move semantics allowed?
Move semantics let you safely transfer ownership of a resource from one object to another. It ensures that at any given time, exactly one object clearly owns the resource, preventing issues like accidentally deleting something twice or creating unwanted copies.

What happens if you don't define a destructor?
If you don't define a destructor, the dynamically allocated object won't ever get deleted, causing it to remain in memory permanently. Over time, these objects build up, consuming more and more memory. Eventually, this can cause performance issues or even make your system run out of available memory.

