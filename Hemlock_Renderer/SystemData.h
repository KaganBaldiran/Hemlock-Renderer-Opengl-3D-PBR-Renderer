#pragma once

#ifdef WINDOWS

#include <libcpuid.h>
#include <libcpuid_types.h>
#include <libcpuid_types.h>
#include "Log.h"

int InitCpuid(cpu_id_t &CPUdata)
{
	if (!cpuid_present())
	{
		LOG_ERR("CPU is not supported!");
		return -1;
	}

	cpu_raw_data_t raw;  

	if (cpuid_get_raw_data(&raw) < 0)
	{
		LOG_ERR("Cannot fetch CPUID raw data! :: " << cpuid_error());
		return -2;
    }

	if (cpu_identify(&raw, &CPUdata) < 0)
	{
		LOG_ERR("CPU identification failed! :: " << cpuid_error());
		return -3;
	}
}

int GetCPUusage()
{
	return cpu_clock_measure(200, 0);
}

#elif LINUX
 

#endif // WINDOWS
