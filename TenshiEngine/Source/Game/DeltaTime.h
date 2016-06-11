#pragma once


#include <windows.h>

class DeltaTime{
protected:

	DeltaTime(){}
	virtual ~DeltaTime(){}

public:
	virtual float GetDeltaTime() const = 0;
	virtual float GetNoScaleDeltaTime() const = 0;
	virtual float GetTimeScale() const = 0;
	virtual void SetTimeScale(float sclae) = 0;

private:

};

class EngineDeltaTime : public DeltaTime{
public:

	EngineDeltaTime();
	~EngineDeltaTime();

	void Tick();

	float GetDeltaTime() const override;
	float GetNoScaleDeltaTime() const override;
	float GetTimeScale() const override;
	void SetTimeScale(float sclae) override;
private:


	LARGE_INTEGER mFreq;
	LARGE_INTEGER mStart;

	float mNoScaleDeltaTime;
	float mDeltaTime;
	float mScale;
};