#pragma once

class GameEngineObject{
public:
	virtual ~GameEngineObject(){}
	virtual void Update(float deltaTime){ (void)deltaTime; }
};