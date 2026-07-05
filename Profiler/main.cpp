#include "Profiler.h"
#include "PercentileObserver.h"
#include "HistogramObserver.h"
#include "ScopedTimer.h"

#include <iostream>
#include <numeric>
#include <vector>

namespace
{
long long sumInts(const std::vector<int>& v)
{
    return std::accumulate(v.begin(), v.end(), 0LL);
}
} // namespace

int main()
{
    Profiler profiler;

    // Attach BEFORE registering probes, observers only hear
    // onProbeRegistered for probes registered after they attach.
    PercentileObserver percentiles;
    HistogramObserver histograms;

    auto handle = profiler.Attach(&percentiles);
    auto handle2 = profiler.Attach(&histograms);

    Probe& sumProbe = profiler.RegisterProbe("sum/int");

    std::vector<int> ints(10000);
    std::iota(ints.begin(), ints.end(), 0);

    long long witness = 0;                    // witness just to stop compiler eliminating code
    for (int run = 0; run < 10000; ++run)
    {
        ScopedTimer t(sumProbe);
        witness += sumInts(ints);
    }

    profiler.Report();
    std::cout << "(witness " << witness << ")\n";
}