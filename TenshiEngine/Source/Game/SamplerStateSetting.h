#pragma once

#include <vector>


struct ID3D11SamplerState;


class SamplerStateSetting {
public:
	SamplerStateSetting();
	~SamplerStateSetting();

	void Initialize();
	void Release();
	void Setting(ID3D11DeviceContext* context);
	void Free(ID3D11DeviceContext* context);


	std::vector<ID3D11SamplerState*> m_Saplers;

private:
	//ÉRÉsÅ[ã÷é~
	SamplerStateSetting(const SamplerStateSetting&) = delete;
	SamplerStateSetting& operator=(const SamplerStateSetting&) = delete;
};