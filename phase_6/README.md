# Example message to server
```
CHALLENGE_RESPONSE <CHALLENGE_ID> <TARGET_TICKER> <BID> <ASK> <TraderName>
```

# Example message from server
```
SEC|SEC0007|BID|92.9962|ASK|93.761
SEC|SEC0006|BID|76.6074|ASK|77.2132
SEC|SEC0005|BID|64.642|ASK|65.0693
CHALLENGE_ID:5
TARGET:SEC0003
```


# XPS details
 NIC vendor 
```
imo@imogene:~/cpp/C-_group_7$ lspci | grep -i network
02:00.0 Network controller: Qualcomm Atheros QCA6174 802.11ac Wireless Network Adapter (rev 32)
```

Num Cores
```
cat /proc/cpuinfo | grep processor | wc -l
8
```


# Improvements we might want to consider
- recvmmsg or non-kernel networking
- Use Non-Blocking I/O
