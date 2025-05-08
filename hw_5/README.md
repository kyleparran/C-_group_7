# Optimized Hash Table 
```
Benchmarkings with 10000 operations, and 10000 max Cap
operation,HashTable,unordered_map,diff
insertion,0.0458019,0.00165698,0.0441449
lookup,0.0174311,0.00104496,0.0163862
removal,0.0421454,0.00145619,0.0406892

Benchmarkings with 10000 operations, and 100000 max Cap
operation,HashTable,unordered_map,diff
insertion,0.0010947,0.00158596,-0.000491263
lookup,0.000775681,0.000659507,0.000116174
removal,0.00152661,0.00103846,0.000488153

Benchmarkings with 10000 operations, and 1000000 max Cap
operation,HashTable,unordered_map,diff
insertion,0.00153694,0.00211853,-0.000581586
lookup,0.00122495,0.00112214,0.000102814
removal,0.00223299,0.00156434,0.000668648
```

Overall the two have comparable performance. The custom implementation has much poorer results which using a small max capacity. Insertion seems to be much better for the custom Robin Hood hashing-based hash table.


# Priority Queue
Benchmarked with 10000 operations
```
Benchmarking HeapPriorityQueue with 10000 operations
operation,HeapPriorityQueue,priority_queue,diff
insertion,0.000682318,0.00100326,-0.000320938
extraction,0.0029394,0.0030976,-0.000158204

Benchmarking HeapPriorityQueue with 100000 operations
operation,HeapPriorityQueue,priority_queue,diff
insertion,0.0059996,0.00969049,-0.0036909
extraction,0.0386225,0.0402981,-0.00167562
```

Overall the custom implementation is faster than the STL implementation for both insertion and extraction. 


# Moving Average
Benchmarked with 10000 operations
```
Benchmarking with 10000 data points and window size 100
operation,SIMD,Normal,diff
movingAverage,0.000113373,0.000117543,-4.17e-06

Benchmarking with 10000 data points and window size 1000
operation,SIMD,Normal,diff
movingAverage,9.7572e-05,0.000111463,-1.3891e-05

Benchmarking with 100000 data points and window size 100
operation,SIMD,Normal,diff
movingAverage,0.0011658,0.0013564,-0.000190594

Benchmarking with 100000 data points and window size 1000
operation,SIMD,Normal,diff
movingAverage,0.0011924,0.00125218,-5.9782e-05
```

Overall the SIMD implementation is just slightly faster than the normal implementation. The difference is more notizable when we have a small window size and a large number of data points. 


