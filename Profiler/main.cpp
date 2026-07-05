#include "Probe.h"
#include "ScopedTimer.h"
#include "Report.h"
#include <numeric> 
#include <vector>
#include <iostream>

namespace 
{    
long long sumInts(const std::vector<int>& v) {
    return std::accumulate(v.begin(), v.end(), 0LL);
}
} // namespace

int main() {
    std::vector<int> ints(10000);
    std::iota(ints.begin(), ints.end(), 0);   

    Probe sumProbe{ "sum/int" };

    long long witness = 0;
    for (int run = 0; run < 10000; ++run) {
        ScopedTimer t(sumProbe);
        witness += sumInts(ints);
    }

    printReport(sumProbe);
    std::cout << "(witness " << witness << ")\n";
}