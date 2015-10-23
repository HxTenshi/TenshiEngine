
#include "MySTL/Ptr.h"
#include "Graphic/Model/Model.h"

#include "IComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Graphic/RenderTarget/RenderTarget.h"

class PostEffectHelper{
public:
	static void SetCurrentTarget(RenderTarget* target){
		mCurrentRenderTarget = target;
	}
	static RenderTarget* CurrentTarget(){
		return mCurrentRenderTarget;
	}
private:
	static RenderTarget* mCurrentRenderTarget;
};


class PostEffectComponent :public Component{
public:


	void Initialize() override;
	void EngineUpdate() override;
	void Update() override;

	void Finish() override;

	void PostDraw();

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;
private:

	RenderTarget mRenderTarget;

	Model mModelTexture;

	Material mMaterial;

	std::string mShaderName;

};