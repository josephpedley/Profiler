#pragma once

#include "LatencyObserver.h"
#include "Probe.h"

#include <deque>
#include <vector>

class Profiler 
{
public:
	Probe& RegisterProbe(std::string name);

	class Handle {
	public:
		Handle() = default;
		~Handle() { reset(); }

		// Handles born in attach so must be moveable
		Handle(Handle&& other) noexcept {
			m_prof = std::exchange(m_prof, nullptr);
			m_observer = std::exchange(m_observer, nullptr); }

		Handle& operator=(Handle&& other) noexcept {
			if (this != &other)
			{
				reset();
				m_prof = std::exchange(m_prof, nullptr);
				m_observer = std::exchange(m_observer, nullptr); 
			}
			return *this; }

		Handle(const Handle& other) = delete;
		Handle& operator=(const Handle& other) = delete;

		void reset()
		{
			if (m_prof)
			{
				m_prof->detach(m_observer);
			}

			m_prof = nullptr;
		}

	private:
		friend class Profiler;
		Handle(Profiler* profiler, LatencyOsberver* latencyObserver)
			: m_prof(profiler)
			, m_observer(latencyObserver)
		{}
		Profiler * m_prof = nullptr;
		LatencyOsberver* m_observer = nullptr;
	};

	Handle Attach(LatencyOsberver* observer);

	template <typename Fn>
	void Notify(Fn&& fn)
	{
		for (auto* observer : m_observers)
		{
			fn(*observer);
		}
	}

	void Report();

private:
	void detach(LatencyOsberver* observer);
	std::deque<Probe> m_probes;
	std::vector<LatencyOsberver*> m_observers;
};
