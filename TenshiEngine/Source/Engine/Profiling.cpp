#include "Profiling.h"

Profiling staticProfiling;

decltype(Profiling::mTickMap) Profiling::mTickMap;
decltype(Profiling::mFreq) Profiling::mFreq;



Profiling::Profiling(){
	QueryPerformanceFrequency(&mFreq);
}
void Profiling::TimeStamp(const std::string& str, double tick){
	mTickMap[str] = tick / (double)mFreq.QuadPart;
}
Counter Profiling::Start(const std::string& str){
	return Counter(str);
}

double Profiling::GetTime(const std::string& str){
	return mTickMap[str];
}

Counter::Counter(){
	memset(&start, 0, sizeof(start));
}

Counter::Counter(const std::string& name)
	: name(name){
	memset(&start, 0, sizeof(start));
	QueryPerformanceCounter(&start);
}
Counter::~Counter(){
	if (start.QuadPart == 0)return;
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	Profiling::TimeStamp(name, (double)(end.QuadPart - start.QuadPart));
};