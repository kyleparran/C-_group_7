HW 2: Group 7
Kyle Parran and Scott Turro

Below is a write up summarizing:

- Which signal triggered the most orders?
- What could you optimize further?
- How would your code behave with 10x more data?

As illustrated in the provided screenshot, Signal 2 was by far the most frequently triggered, with approximately 93,000 instances where it generated an order. Signal 2 operates as a basic mean-reversion indicator: it calculates the recent average price, signaling a buy order when the current price drops below 98% of this average and a sell order when it rises above 102% of the average. Given the broad price range (100–200), it is not surprising that this signal consistently triggered many buy orders, as its threshold for activation is relatively modest.

The attached code could significantly benefit from further optimizations, such as parallelization and execution across multiple CPU cores, since it currently runs in a single-threaded manner. Additionally, performance gains might be realized by explicitly inlining some of the more frequently invoked functions, including `updateHistory` and `getAvg`.

Given the single-threaded design, the runtime is expected to scale linearly with an increase in data volume. Testing with one million ticks (instead of the original one hundred thousand) confirmed this expectation, as both runtime and the number of generated orders increased approximately tenfold.