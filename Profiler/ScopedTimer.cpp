#include <chrono>
#include <vector>
#include <thread>
#include <iostream>
#include <numeric>

std::vector<uint64_t> g_samples;
std::vector<int> g_ints(10000);


class ScopedTimer
{
public:
	ScopedTimer()
		: start_(std::chrono::steady_clock::now())
	{ }

	~ScopedTimer()
	{
		auto end = std::chrono::steady_clock::now();
		auto timeNs = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count();
		g_samples.push_back(timeNs);
	}

	// timer should not be copyable
	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator = (const ScopedTimer&) = delete;
	
private:
	std::chrono::steady_clock::time_point start_;
};

void doWork()
{
	ScopedTimer t;
	int sum = std::accumulate(g_ints.begin(), g_ints.end(), 0);
	std::cout << sum << "\n";
}

int main()
{

	for (auto i = 0; i < 10000; ++i)
	{
		g_ints.push_back(i);
	}

	doWork();

	for (auto ns : g_samples)
	{
		std::cout << ns << "ns \n";
	}
}


