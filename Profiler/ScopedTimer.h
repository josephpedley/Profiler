#pragma once

#include "Probe.h"

#include <chrono>

class ScopedTimer
{
public:
	explicit ScopedTimer(Probe& probe)
		: start_(std::chrono::steady_clock::now())
		, m_probe(probe)
	{	}

	~ScopedTimer()
	{
		auto end = std::chrono::steady_clock::now();
		m_probe.Record(static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start_).count()));
	}

	// timer should not be copyable
	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator = (const ScopedTimer&) = delete;

private:
	std::chrono::steady_clock::time_point start_;
	Probe& m_probe;
};
