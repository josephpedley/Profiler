#pragma once

#include "LatencyObserver.h"
#include "Profiler.h"

class PercentileObserver : public LatencyOsberver
{
public:
	void onProbeRegistered(size_t probeId, const std::string& name) override;
	void onSample(size_t probeId, uint64_t nsTime) override;
	void onReportRequested() override;

private:
	std::vector<std::string> m_names;
	std::vector<std::vector<uint64_t>> m_samples;
};