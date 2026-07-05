#include "Profiler.h"
#include <algorithm>

Probe& Profiler::RegisterProbe(std::string name)
{
	const size_t id = m_probes.size();
	Probe& probe = m_probes.emplace_back(std::move(name));

	Notify([&](LatencyOsberver& observer) {
		observer.onProbeRegistered(id, probe.Name());
		});

	return probe;
}

Profiler::Handle Profiler::Attach(LatencyOsberver* observer)
{
	m_observers.push_back(observer);
	return Handle{ this, observer };
}

void Profiler::detach(LatencyOsberver* observer)
{
	auto it = std::ranges::find(m_observers, observer);
	if (it != m_observers.end())
	{
		*it = nullptr;
	}
}

void Profiler::Report()
{
	for (size_t id = 0; id < m_probes.size(); ++id)
	{
		for (auto nsTime : m_probes[id].Samples())
		{
			Notify([&](LatencyOsberver& observer) {
				observer.onSample(id, nsTime);
				});
		}
		Notify([](LatencyOsberver& observer) {
			observer.onReportRequested();
			});
	}
}
