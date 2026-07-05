Tested profiler with sleep this thread for 1 ms. Expected values for this test to show 1ms. Instead they were ~16ms.
Lesson:
Windows scheduler has a timer resolution of around 16ms.

Tested profiler by summing vector of 10000 ints. Got either exactly 0 or 100 ms.
Lesson:
Windows uses QueryPerformanceCounter for steady_clock. Resolution must be 100ns. Also printed the sum to get a much larger ns value. Previously the compiler knew the loop had no observable effect
and deleted it. Printing it forced the work to happen.


