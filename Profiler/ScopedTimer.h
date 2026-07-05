#pragma once

#include "Probe.h"

#include <chrono>

class ScopedTimer
{
public:
	explicit ScopedTimer(Probe& probe)
		: m_start(std::chrono::steady_clock::now())
		, m_probe(probe)
	{	}

	~ScopedTimer()
	{
		auto end = std::chrono::steady_clock::now();
		m_probe.Record(static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start).count()));
	}

	// timer should not be copyable
	ScopedTimer(const ScopedTimer&) = delete;
	ScopedTimer& operator = (const ScopedTimer&) = delete;
	
	// can't be movable anyway as compiler won't give us any due to custom destructor so don't need to define
	// Could be nice to add delete for readability but not really needed

private:
	std::chrono::steady_clock::time_point m_start;
	Probe& m_probe;
};
