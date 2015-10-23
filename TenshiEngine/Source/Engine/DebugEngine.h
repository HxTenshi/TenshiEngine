#pragma once
#include <string>

class DebugEngine{
public:
	virtual void Log(const std::string& log) = 0;
};