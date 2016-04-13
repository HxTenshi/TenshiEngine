
#include "MySTL/Ptr.h"
#include "Graphic/Model/Model.h"

#include "IComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Graphic/RenderTarget/RenderTarget.h"

class PostEffectComponent :public Component{
public:


	void Initialize() override;
	void EngineUpdate() override;
	void Update() override;

	void Finish() override;

	void PostDraw();

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;
private:

	RenderTarget mRenderTarget;

	Model mModelTexture;

	Material mMaterial;

	std::string mShaderName;

};