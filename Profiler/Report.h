#pragma once
#include "Probe.h"

#include <algorithm>
#include <iostream>

void printReport(const Probe& probe)
{
	auto s = probe.Samples(); // copy so that samples can be sorted
	std::sort(s.begin(), s.end());

	auto percentile = [&](double quantile) {
		return s[static_cast<size_t>(quantile * (s.size() - 1))];
		};

	std::cout << "=== " << probe.Name() << " ===\n"
		<< "p50: " << percentile(0.5) << "ns \n"
		<< "p99: " << percentile(0.99) << "ns \n"
		<< "p99.9: " << percentile(0.999) << "ns \n"
		<< "p99.99: " << percentile(0.9999) << "ns \n"
		<< "worst case: " << s.back() << "ns \n";
}
