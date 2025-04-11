Group 7
Scott Turro and Kyle Parran

## Momentum

Our momentum strategy takes trades only after detecting two consecutive price movements in the same direction (either upward or downward). Specifically, we evaluate momentum by checking the current price change against the previous price change:

- If the last price increased, we want to see if the current price also increased:
  - `(currPrice - lastPrice) > 0`
- If the last price decreased, we want to confirm another decrease:
  - `(currPrice - lastPrice) < 0`, or equivalently, `-(currPrice - lastPrice) > 0`

To manage this efficiently, we store the direction of the last price movement as an integer state:
- `1`: if the previous change was positive (upward)
- `-1`: if the previous change was negative (downward)

If our trade is executed, this indicates the current price moved consistently with our stored state, so no state change is needed. Otherwise, we reverse the stored state to reflect the opposite momentum direction.

## Code Optimization

Initially, we started with a straightforward implementation and identified several optimization opportunities:

- **Parsing Price Data:**
  - Avoid unnecessary creation and manipulation of strings by directly operating on character buffers.

- **Trade Detection Logic:**
  - Efficient momentum detection by storing the sign of the previous change rather than recalculating direction repeatedly.

- **Variable Management:**
  - Use pointers or references for frequently accessed variables such as `currPrice` and `lastPrice` to minimize overhead.
  - Leverage direct memory operations; for example, store `currPriceID` directly in the `priceBuffer` to eliminate extra copying.

- **Initialization Phase:**
  - Allow the first two prices to "burn-in" the system, setting initial momentum direction without prematurely triggering trades.



## Useful commands

If you stop the server or client abruptly, the port may not be free. You can look up the processes using the ports using

```bash
sudo lsof -i :12345
```
Kill HFTServer or HFTClient using its associated pid
```bash
sudo kill -9 202980
```

**In the future** Use `ctrl+c` to stop a program instead of `ctrl+z` which stops the program and puts it in the background.

