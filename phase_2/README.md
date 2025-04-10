# Momementum
We take trades if we detect 2 consecutive increases or decreases in prices. Therefore we can determine trades based on the last change and the current change in price. If we just saw a increase in prices, we want `(currPrice - lastPrice) > 0` Otherwise we want `(currPrice - lastPrice) < 0 -> -(currPrice - lastPrice) > 0`. 

To do this we store the state of the last change as an int 
- 1: if last change was positive
- -1: if last change was negative

If we trade our price then we saw the same type of change and we do not have to change our state. Otherwise we flip the sign of our state.


# Code Optimization
We started with a simple implementation and then looked for areas of improvement. The following came to mind
- Parsing price
    - Avoid creating and working with strings
- Trade Detection
    - See above
- Variable access
    - Use pointers for currPrice and lastPrice
    - currPriceID is stored in priceBuffer -> no need to copy
- Other
    - Burn in 2 prices



# Usefull commands

If you stop the server or client ubrutly, the port may not be free. You can look up the processes using the ports using

```
sudo lsof -i :12345
```
Kill HFTServer or HFTClient using its associated pid
```
sudo kill -9 202980
```

**In the future** Use `ctrl+c` to stop a program instead of `ctrl+z` which stops the program and puts it in the background.