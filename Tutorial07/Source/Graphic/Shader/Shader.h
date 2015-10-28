#pragma once

#include "PixelShader.h"
#include "VertexShader.h"
#include "MySTL/Ptr.h"

class Shader{
public:
	Shader(){

	}
	~Shader(){

	}

	void Create(const char* fileName){
		mVertexShader = shared_ptr<VertexShader>(new VertexShader());
		mPixelShader = shared_ptr<PixelShader>(new PixelShader());

		mVertexShader->Create(fileName, "VS");
		mPixelShader->Create(fileName);
	}

	void SetShader() const{
		if (mVertexShader)
			mVertexShader->SetShader();
		if (mPixelShader)
			mPixelShader->SetShader();
	}

private:
	shared_ptr<PixelShader> mPixelShader;
	shared_ptr<VertexShader> mVertexShader;
};