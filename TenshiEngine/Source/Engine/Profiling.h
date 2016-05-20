#pragma once

#include <windows.h>
#include <unordered_map>


struct Counter{
	Counter();
	Counter(const std::string& name);
	~Counter();

	std::string name;
	LARGE_INTEGER start;
};


class Profiling{
public:
	Profiling();
	static void TimeStamp(const std::string& str, double tick);
	static Counter Start(const std::string& str);
	static double GetTime(const std::string& str);
private:
	static std::unordered_map <std::string, double > mTickMap;
	static LARGE_INTEGER mFreq;
};