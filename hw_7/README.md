# Homework 5
### Group 7: Kyle Parran & Scott Turro

Settings Used:
```
constexpr int numProducers=50;
constexpr int ordersPerProducer=100;
constexpr int numConsumers=40;
```

## Initial Results
Vaired alot and would sometimes jump take 2-3 times as long
```
--- Analytics --- (Good case)
Avg Latency:  22277.5 µs
Std Latency:  7728.62 µs
Q1 | Q2 | Q3: 16479 | 23567 | 28488 µs²

--- Analytics --- (Bad case)
Avg Latency:  35694.7 µs
Std Latency:  9311.18 µs
Q1 | Q2 | Q3: 29214 | 37707 | 43986 µs²


--- Analytics ---  (Rare worst case)
Avg Latency:  61505.9 µs
Std Latency:  30280.8 µs
Q1 | Q2 | Q3: 26933 | 82707 | 87632 µs²

```
In each of the statistics, the mean is less then the median, so the distributed is skewed left. So some orders are taking much longer, most likely because they experiencing some kind of blockage.


# Results with Latch 
Faster, much more consistent, and symmetric. Latch included from hereon
```
--- Analytics ---
Avg Latency:  22245.1 µs
Std Latency:  10603.3 µs
Q1 | Q2 | Q3: 13133 | 22943 | 30547 µs²
```



# Results with processingSemaphore
The more semaphores we added, the more inconsistent and slower it got
```
--- Analytics --- (10 semaphores)
Avg Latency:  68728.1 µs
Std Latency:  35465.6 µs
Q1 | Q2 | Q3: 40100 | 69847 | 99332 µs²

--- Analytics ---  (20 semaphores)
Avg Latency:  41744.7 µs
Std Latency:  21629.2 µs
Q1 | Q2 | Q3: 22168 | 43667 | 60858 µs²


--- Analytics ---  (30 semaphores)
Avg Latency:  32757.3 µs
Std Latency:  16801.1 µs
Q1 | Q2 | Q3: 18652 | 34864 | 46974 µs²
```

## Results with syncBarrier
Performance was significiantly worse
```
Avg Latency:  63945 µs
Std Latency:  32571.5 µs
Q1 | Q2 | Q3: 36473 | 66487 | 93048 µs²
```

## Results with using ConcurrentQueue for completedOrders
We replaced the vector+lock with ConcurrentQueue for the completed Orders. This lead to a noticable improvement
```
--- Analytics ---
Avg Latency:  16782.8 µs
Std Latency:  10096 µs
Q1 | Q2 | Q3: 7371 | 15339 | 25675 µs²

```

## Results with using ConcurrentQueue for completedOrders
Noticable improvement
```
--- Analytics ---
Avg Latency:  17633.8 µs
Std Latency:  10113.5 µs
Q1 | Q2 | Q3: 8402 | 18012 | 25585 µs²

```