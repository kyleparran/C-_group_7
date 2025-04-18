## Group 7 : Phase 3
Kyle Parran & Scott Turro



## Quick Overview

- **Market Snapshot**: Tracks the best available bid and ask prices in real-time.
- **Order Manager**: Manages orders you place—tracking their status (new, partially filled, completed, or cancelled).
- **Strategy Execution**: Makes basic trading decisions based on current market snapshots and executes orders accordingly.

### Overview of the System Architecture

The system is structured into three key parts that work closely together. First, the **Market Snapshot** component continuously tracks the current best prices buyers and sellers offer in the market, updating instantly as new data comes in. Next, the **Order Manager** keeps a clear record of every order we've placed, monitoring whether they've been filled, partially filled, or canceled. Lastly, the **Strategy Logic** sits on top of both these pieces—it watches market conditions and makes quick decisions, placing trades automatically whenever certain conditions are met, like price improvements or worsening market conditions.

### How Memory is Managed Safely

Memory safety in this trading system is handled very carefully. Rather than manually allocating and freeing memory, the system uses smart pointers (`std::unique_ptr`) which automatically handle the lifetime of objects which means that the moment an order is canceled, fully filled, or a price level disappears, the associated memory is cleaned up instantly and safely. Additionally, the RAII pattern ensures every resource allocated is automatically released when no longer needed, completely eliminating things such as memory leaks, dangling pointers, or double deletes.

## Strategy
We implemented a simple strategy that keeps an eye on the market for two specific triggers: if the ask price gets worse (goes higher), we quickly place a sell order at the previous, better price; and if the bid improves by exactly two cents or more, we immediately sell at this new, higher bid. It carefully tracks order fills, automatically removing fully executed orders and avoiding duplicate trades. The entire system prioritizes memory safety, using modern C++ practices for memory management.

## Project Structure

Here's how the project files are organized:

```
.
├── CMakeLists.txt          
├── config.yaml             
├── sample_feed.txt         
├── include/
│   ├── feed_parser.h
│   ├── market_snapshot.h
│   ├── order_manager.h
│   └── logger.h
├── src/
│   ├── feed_parser.cpp
│   ├── market_snapshot.cpp
│   ├── order_manager.cpp
│   └── logger.cpp
└── main.cpp                
```

## What Each Module Does

### 1. Market Snapshot

Keeps track of the market's best bids and asks:

- Stores prices in efficient, sorted containers for quick look-ups.
- Automatically cleans up price levels with zero quantity using smart pointers, avoiding memory leaks.

### 2. Order Manager

- Assigns unique IDs to orders.
- Tracks the lifecycle of orders, including placement, fills, and cancellations.
- Removes orders automatically upon full execution or cancellation, ensuring safe memory management.

### 3. Strategy Execution

- Continuously monitors market conditions.
- Automatically places orders based on the Strategy outlined above.

## How to Build and Run

To build and execute using CMAKE:

```bash
cmake -S . -B build
cmake --build build
./main
```

For additional memory safety checks:

```bash
cmake -B build-asan -DCMAKE_CXX_FLAGS="-fsanitize=address -g" .
cmake --build build-asan
./build-asan/main
```

Run Valgrind (if you're on Linux) to further verify memory correctness:

```bash
cd .. build-asan
valgrind --leak-check=full ./main
```

## Example Program Output

If you don't modify the sample text, it should have an output similar to below:

```
[Market] New Best Bid: 100.10 x 300
[Market] New Best Ask: 100.20 x 250
[Market] New Best Bid: 100.15 x 200
[Market] New Best Ask: 100.18 x 100
[Market] Best Ask: 100.18 removed
[Market] New Best Ask: 100.25 x 200

[Strategy] Placing SELL order at 100.18 x 50 (ID = 1)
[Execution] Order 1 filled: 10
[Order] Order 1 partially filled: 10 / 50
[Execution] Order 1 filled: 20
[Order] Order 1 partially filled: 30 / 50

[Strategy] Placing SELL order at 100.17 x 100 (ID = 2)
[Execution] Order 2 filled: 50
[Order] Order 2 partially filled: 50 / 100
[Execution] Order 2 filled: 50
[Order] Order 2 completed (100 / 100) and removed
```

All output is also logged to `output.log` 


