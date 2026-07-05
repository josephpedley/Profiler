#include "PercentileObserver.h"

#include <algorithm>
#include <iostream>

void PercentileObserver::onProbeRegistered(size_t probeId, const std::string& name)
{
	m_names.push_back(name);
	m_samples.emplace_back();
}

void PercentileObserver::onSample(size_t probeId, uint64_t nsTime)
{
	m_samples[probeId].push_back(nsTime);
}

void PercentileObserver::onReportRequested()
{
	for (size_t id = 0; id < m_samples.size(); ++id)
	{
		auto samples = m_samples[id];
		if (samples.empty()) continue;

		std::ranges::sort(samples);

		auto percentile = [&](double quantile) {
			return samples[static_cast<size_t>(quantile * (samples.size() - 1))];
			};

		std::cout << "=== " << m_names[id] << " ===\n"
			<< "p50: " << percentile(0.5) << "ns \n"
			<< "p99: " << percentile(0.99) << "ns \n"
			<< "p99.9: " << percentile(0.999) << "ns \n"
			<< "p99.99: " << percentile(0.9999) << "ns \n"
			<< "worst case: " << samples.back() << "ns \n";
	}
}