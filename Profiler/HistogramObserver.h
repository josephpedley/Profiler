#pragma once

#include "LatencyObserver.h"
#include "Profiler.h"

#include <array>

class HistogramObserver : public LatencyOsberver
{
public:
	void onProbeRegistered(size_t probeId, const std::string& name) override;
	void onSample(size_t probeId, uint64_t nsTime) override;
	void onReportRequested() override;

private:
	static constexpr size_t kBucketCount = 65;
	std::vector<std::string> m_names;
	std::vector<std::array<uint64_t, kBucketCount>> m_buckets;
};
