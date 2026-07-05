#include <chrono>

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
	}

	// timer should not be copyable
	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator = (const ScopedTimer&) = delete;
	
private:
	std::chrono::steady_clock::time_point start_;
};