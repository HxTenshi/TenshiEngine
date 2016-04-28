#pragma once
#include <string>
#include "DebugEngine.h"

class DebugSystem : public DebugEngine{
public:
	void Log(const std::string& log) override;
};