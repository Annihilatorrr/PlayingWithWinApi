#pragma once

#include  <cstdint>
struct PerfRawData
{
	unsigned int processId;
	std::uint64_t  percentProcessorTime;
	std::uint64_t  frequency100Ns;
};

