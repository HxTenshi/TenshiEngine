#include "DeltaTime.h"


EngineDeltaTime::EngineDeltaTime(){

	QueryPerformanceFrequency(&mFreq);

	memset(&mStart, 0, sizeof(mStart));

	mNoScaleDeltaTime = 0.0f;
	mDeltaTime = 0.0f;
	mScale = 1.0f;
}
EngineDeltaTime::~EngineDeltaTime(){

}
void EngineDeltaTime::Reset(){
	mNoScaleDeltaTime = 0.0f;
	mStart.QuadPart = 0;
}
void EngineDeltaTime::Tick(){

	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	if (mStart.QuadPart != 0)
		mNoScaleDeltaTime = ((double)end.QuadPart - (double)mStart.QuadPart) / (double)mFreq.QuadPart;
	mDeltaTime = mNoScaleDeltaTime * mScale;

	mDeltaTime = min(mDeltaTime, 4.0f/60.0f);

	mStart = end;
}

float EngineDeltaTime::GetDeltaTime()const{
	return mDeltaTime;
}
float EngineDeltaTime::GetNoScaleDeltaTime()const{
	return mNoScaleDeltaTime;
}
float EngineDeltaTime::GetTimeScale()const{
	return mScale;
}
void EngineDeltaTime::SetTimeScale(float scale) {
	mScale = scale;
}