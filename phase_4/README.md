
# todo


# Assumptions
We manage a order book for our clients. This is assumed to be fixed in time. We monitor the
market and will enact trades on behalf of our clients.



📝 README (design overview, architecture, build/run instructions) 	✅
📊 Benchmark report (tick-to-trade latency statistics) 	✅
🧪 Performance test results + brief analysis 	✅
📈 Architecture diagram (system/module/class flow) 	✅
🎥 Video demo showing the system working 	✅

# Experiments
🧪 Experiments to Run
Variable 	Experiment 	What to Observe
Smart vs raw pointers 	Swap unique_ptr with raw pointers 	Memory safety vs overhead
Memory alignment 	Add/remove alignas(64) 	Cache behavior differences
Custom allocator 	Use memory pool vs new/delete 	Allocation speed
Container layout 	Flat array vs map/multimap 	Access time difference
Load scaling 	1K, 10K, 100K ticks 	Latency consistency under pressure