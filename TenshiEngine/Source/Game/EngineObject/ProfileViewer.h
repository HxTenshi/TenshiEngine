#pragma once

#include "GameEngineObject.h"
#include <string>
#include <vector>
#include "MySTL/ptr.h"
class ProfileBar;

class ProfileViewer : public GameEngineObject{
public:
	ProfileViewer();
	~ProfileViewer();
	void Update() override;

	void AddBar(const std::string& name, int CPU, const XMFLOAT4& color);

private:

	ProfileViewer& operator = (const ProfileViewer&);

	std::vector<shared_ptr<ProfileBar>> mProfileBars[2];
};