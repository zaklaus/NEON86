#include "StdAfx.h"
#include "system.h"

FLOAT GetTime(BOOL flush)
{
	static LARGE_INTEGER perf_freq = {0};
	static LARGE_INTEGER perf_counter = {0};
	LARGE_INTEGER counter;
	if (!perf_counter.QuadPart)
	{
		QueryPerformanceFrequency(&perf_freq);
		QueryPerformanceCounter(&perf_counter);
	}

	QueryPerformanceCounter(&counter);

    if (flush)
    {
        QueryPerformanceCounter(&perf_counter);
        counter.QuadPart = perf_counter.QuadPart;
    }

	return (counter.QuadPart - perf_counter.QuadPart) / (FLOAT)(perf_freq.QuadPart);
}

#include <unordered_map>
static std::unordered_map<LPVOID, DWORD> gMemoryMap;

VOID neon_mempeak_update()
{
	if (gMemUsed > gMemPeak)
		gMemPeak = gMemUsed;
}

VOID neon_memreset()
{
	// todo: avoid doing this
	gMemUsed = gMemPeak = 0;
	gMemoryMap.clear();
}

LPVOID neon_malloc(DWORD size)
{
	gMemUsed += size;
	neon_mempeak_update();

	LPVOID mem = malloc(size);
	gMemoryMap[mem] = size;
	return mem;
}

LPVOID neon_realloc(LPVOID mem, DWORD newSize)
{
	if (gMemoryMap.find(mem) != gMemoryMap.end())
	{	
		gMemUsed += (newSize - gMemoryMap[mem]);
		neon_mempeak_update();
		gMemoryMap.erase(mem);
	}

	LPVOID newMem = realloc(mem, newSize);
	
	gMemoryMap[newMem] = newSize;
	return newMem;
}

VOID neon_free(LPVOID mem)
{
    if (gMemoryMap.find(mem) != gMemoryMap.end())
    {
        gMemUsed -= gMemoryMap[mem];
        gMemoryMap.erase(mem);
    }

	free(mem);
}

DWORD gMemUsed = 0;
DWORD gMemPeak = 0;