#pragma once
#include <vector>
#include <string>

class Probe
{
public:
	explicit Probe(std::string name)
		: m_name(std::move(name))
	{
		m_samples.reserve(1 << 20);
	}

	// Probe should not be copyable
	Probe(const Probe&) = delete;
	Probe& operator=(const Probe&) = delete;

	/* Probes hand out references of themselves to scopedTimer and registerProbe
	their addresses therefore must be stable for the profilers lifetime and should be unmovable */
	Probe(Probe&& other) = delete;
	Probe& operator=(const Probe&&) = delete;

	void Record (uint64_t nsTime) { m_samples.push_back(nsTime); }
	const std::string& Name() const { return m_name; }
	const std::vector<uint64_t>& Samples() const { return m_samples; }

private:
	std::vector<uint64_t> m_samples;
	std::string m_name;
};
