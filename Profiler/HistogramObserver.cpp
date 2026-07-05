#include "HistogramObserver.h"

#include <algorithm>
#include <iostream>
#include <bit>
#include <iomanip>

void HistogramObserver::onProbeRegistered(size_t probeId, const std::string& name)
{
	m_names.push_back(name);
	m_buckets.push_back({});
}

void HistogramObserver::onSample(size_t probeId, uint64_t nsTime)
{
	m_buckets[probeId][std::bit_width(nsTime)]++;
}

void HistogramObserver::onReportRequested()
{
	for (size_t id = 0; id < m_buckets.size(); ++id)
	{
		auto maxCount = *std::ranges::max_element(m_buckets[id]);
		if (maxCount == 0) continue;

		std::cout << "=== " << m_names[id] << " ===\n";

		for (size_t i = 0; i < kBucketCount; ++i)
		{
			if (m_buckets[id][i] == 0) continue;

			uint64_t lo = (i == 0) ? 0 : uint64_t{ 1 } << (i - 1);

			int barLen = static_cast<int>(50.0 * m_buckets[id][i] / maxCount);
			if (barLen == 0) barLen = 1;

			std::cout << std::setw(10) << lo << "ns | "
				<< std::string(barLen, '#') << ' ' << m_buckets[id][i] << '\n';
		}

	}
}