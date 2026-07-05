# Profiler notes

## Measurements

Tested profiler with sleep_for 1ms. Expected values around 1ms, instead got ~16ms.

Lesson: Windows scheduler wakes sleeping threads on a ~15.6ms tick by default. sleep_for only guarantees a *minimum*, you wake on the next tick that satisfies it. Programs that need finer sleeps call timeBeginPeriod(1) to raise the tick rate (costs battery).

Tested profiler by summing a vector of 10000 ints. Got either exactly 0 or 100 **ns** (not ms, correcting myself here).

Lesson: MSVC steady_clock uses QueryPerformanceCounter, which ticks at ~100ns on my machine. Work shorter than one tick just rounds to 0 or 1 tick. Fix is to run the work N times inside one timed scope and divide by N.

Also had to print the sum to get a real ns value. Compiler knew the loop had no observable effect and deleted the whole thing (as-if rule), so I was timing nothing. Printing the result forces the work to happen. Every Release benchmark needs a witness like this - volatile or DoNotOptimize are the proper versions.

## Language stuff

Mark single parameter constructors as explicit to avoid unintentional implicit conversions.

Base classes must have a virtual destructor, else derived objects deleted through a base pointer only run the base destructor.

Special members:
- Defining a custom destructor suppresses implicit *moves*, but the compiler STILL generates default copies (legacy behaviour, technically deprecated).
- So on a class where the destructor does real work (Handle detaching), I have to explicitly delete the copies or I get memberwise copies and double detach.
- Moves are already suppressed so deleting them too is optional, but did it anyway so the class reads properly without knowing the suppression rules.
- If the class needs to travel (Handle gets returned from attach()), write the moves by hand with std::exchange to empty the source, and mark them noexcept, a vector only uses your move on reallocation if it's noexcept.
- Rule of Zero where possible: if all members clean themselves up, declare nothing (Profiler, PercentileObserver).

emplace_back(args) constructs the object directly inside the container's memory from the constructor args. No temporary, works even with copies deleted.

emplace_back() with no args default constructs the element, I used this to open an empty vector<uint64_t> bucket per probe.

Since C++17 emplace_back returns a reference to the new element, so Probe& p = probes_.emplace_back(std::move(name)) works in one line.

std::ranges has very useful algorithms for clean code rather than messing with .begin() .end() everywhere. ranges::sort(v), ranges::find(v, x).

## Design

Deque was the right container for probes. Probes need a stable address because they give references of themselves out (registerProbe returns Probe&, ScopedTimer holds Probe&) and the container grows.
- Deque grows at the back and never relocates its existing elements so references are never invalidated (actually guaranteed by the standard).
- Vector is one contiguous block, when it grows past capacity it allocates a bigger block elsewhere, moves everything and frees the old buffer and every reference dangles silently.
- Could use reserve but that's just a guess acting as a correctness guarantee, not bulletproof.

Rule from this: if an API hands out references to things it stores, the container choice is part of the API contract.

Ownership map:
- The deque owns the Probes. Every Probe& elsewhere is borrowing.
- Observer list is raw pointers so non-owning, observers are owned by whoever made them.
- Handle is the detach obligation as an object. Only attach() can make one (private ctor), move only so the obligation can relocate but never duplicate, destructor detaches. Turns "remember to detach" into something the compiler enforces.

Event protocol:
- onSample: many per probe, the actual stream
- onProbeRegistered: once per probe
- onReportRequested: once per drain, fired after all samples so things like percentiles can conclude with the full picture
- Have to attach BEFORE registering probes or the observer misses the registrations. No replay for late attachers yet, it's a known sharp edge, maybe v2.

Stuff I'm deliberately not doing yet (mainly because I'm tired and the weekend is over :P):
- Tombstones never get compacted. Fine at this scale, add a dispatching_ flag + erase_if if observer churn ever matters.
- Profiler must outlive all its Handles. Documented contract rather than machinery.
- report() re sends full history every call. Fine for batch, live mode will need per-probe high water marks.
- Histogram observer for a histogram report
- Live stat display updated every 100ms. Would use a jthread for this but also relies on some changes for performance problems this would give.