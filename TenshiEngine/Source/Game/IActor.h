#pragma once

#include "MySTL/ptr.h"

class IActor :public enable_shared_from_this<IActor> {
public:
	virtual ~IActor() {}
	virtual void* _GetScript(const char* name) = 0;

	virtual std::string Name() = 0;
	virtual void Name(const std::string& name) = 0;
};