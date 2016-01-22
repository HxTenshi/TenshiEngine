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
		mVertexShader = make_shared<VertexShader>();
		mVertexShaderAnime = make_shared<VertexShader>();
		mPixelShader = make_shared<PixelShader>();

		if (FAILED(mVertexShader->Create(fileName, "VS"))){
			//MessageBox(NULL, "The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file. VS", "Error", MB_OK);
		}
		mVertexShaderAnime->Create(fileName, "VSSkin");
		mPixelShader->Create(fileName);
	}

	void SetShader(bool UseAnime) const{
		if (UseAnime){
			if (mVertexShaderAnime)
				mVertexShaderAnime->SetShader();
		}else{
			if (mVertexShader)
				mVertexShader->SetShader();
		}
		if (mPixelShader)
			mPixelShader->SetShader();
	}

private:
	shared_ptr<PixelShader> mPixelShader;
	shared_ptr<VertexShader> mVertexShader;
	shared_ptr<VertexShader> mVertexShaderAnime;
};