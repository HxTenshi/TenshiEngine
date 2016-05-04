#pragma once

#include <D3D11.h>

class InputLayout{
public:
	InputLayout();
	~InputLayout();
	HRESULT Init(ID3DBlob* pVSBlob);
	HRESULT Init(ID3DBlob* pVSBlob, const D3D11_INPUT_ELEMENT_DESC* layout, UINT num);
	void SetInputLayout(ID3D11DeviceContext* context)const;
	void Rerease();

private:
	ID3D11InputLayout*	mpVertexLayout;
};