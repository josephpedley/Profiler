# Future plans

Rough order, each is its own session.

## Multithreaded live display

- Drain thread using jthread + stop_token, ticks every 100ms and does all the Notify calls. Observers stay single threaded and don't even know threading exists.
- One SPSC ring per worker thread. Sample = {probeId, nsTime}. Don't share one queue.
- thread_local pointer to the thread's queue, workers call attachThread() once when they start.
- Probes stop storing samples, samples flow through the queues to the observers. Probe becomes basically just a name + id.
- When a ring is full: drop the sample and count it. Don't block (would stall the code being measured), don't grow (allocation ruins latency). Report shows the drop count.
- Shutdown order: join workers first, THEN stop the drain thread so the final sweep catches their last pushes.
- Run with TSan from the start.

## Accuracy

- Swap steady_clock for the cycle counter. now() costs ~20-40ns per call, cycle counter is a few ns. Calibrate cycles -> ns once at startup, store cycles in the sample, convert at drain time not on the hot path. Laptop is ARM64 (CNTVCT) and desktop is x86 (rdtsc) so this needs both.
- Measure the profiler's own overhead with a probe around an empty scope and put the number in the README.
- Add --warmup N to discard the first iterations, cold caches pollute them.
- Thread pinning option.
- selftest mode that prints clock resolution, record overhead and calibration drift at startup.
- Document that this measures wall time (includes preemption, page faults etc) not CPU time.

## Making it usable as a library

- HdrHistogram style bucketing instead of storing every sample. Constant memory, mergeable across threads. Read the design notes first.
- Header only, no dependencies.
- chrome://tracing JSON output so I don't have to build a UI.
- Fix or document the late attach problem, observer attaching after probes are registered misses onProbeRegistered. Could replay registrations to new observers.
- static_assert the noexcept moves so it's guarded.

## Display

- Multi line dashboard, one row per probe, ANSI cursor up to redraw.
- Running max in LiveStatsObserver, jumps visibly when an OS stall lands.
- Add a second probe with actual variance so the display isn't just a flat number.

## Things to point it at

- the SPSC queue i'll use itself, push/pop latency under load.
- std::function vs template dispatch vs virtual call with the cycle counter version.
- Same code on the ARM64 laptop vs the 9800X3D, compare the histograms.

## Repo

- README: what it is, a histogram screenshot, 3 line usage example at the top. Add to design decisions section.
- Keep updating learning log
- Overhead benchmark lives in the repo as runnable code.

## Rules

- Nothing allocates, locks or makes syscalls on the record path.
- Bulk measure anything within ~10x of clock resolution.
- Don't trust run deltas smaller than run to run variance.
- Build the naive version first when the failure teaches something, sanitiser on.