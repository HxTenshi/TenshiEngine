#pragma once

#include "GameEngineObject.h"
#include <string>
#include <vector>
class ProfileBar;

class ProfileViewer : public GameEngineObject{
public:
	ProfileViewer();
	~ProfileViewer();
	void Update(float deltaTime) override;

	void AddBar(const std::string& name, int CPU, const XMFLOAT4& color);

private:

	ProfileViewer& operator = (const ProfileViewer&);

	std::vector<ProfileBar*> mProfileBars[2];
};