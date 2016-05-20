
#include "ComponentFactory.h"

#include "TransformComponent.h"
#include "PhysXComponent.h"
#include "PhysXColliderComponent.h"
#include "TextComponent.h"
#include "CameraComponent.h"
#include "ModelComponent.h"
#include "TextureModelComponent.h"
#include "AnimationComponent.h"
#include "MeshComponent.h"
#include "MeshDrawComponent.h"
#include "ScriptComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "PostEffectComponent.h"
#include "ParticleComponent.h"
#include "Component.h"



decltype(ComponentFactory::mFactoryComponents) ComponentFactory::mFactoryComponents;

class __ComponentFactory : public ComponentFactory{};
__ComponentFactory initialize;

ComponentFactory::ComponentFactory(){
	_NewFunc<CameraComponent>();
	_NewFunc<TransformComponent>();
	_NewFunc<ModelComponent>();
	_NewFunc<TextureModelComponent>();
	_NewFunc<MaterialComponent>();
	_NewFunc<AnimationComponent>();
	_NewFunc<MeshComponent>();
	_NewFunc<MeshDrawComponent>();
	_NewFunc<ParticleComponent>();
	_NewFunc<ScriptComponent>();
	_NewFunc<PhysXComponent>();
	_NewFunc<PhysXColliderComponent>();
	_NewFunc<TextComponent>();
	_NewFunc<PostEffectComponent>();
	_NewFunc<DirectionalLightComponent>();
	_NewFunc<PointLightComponent>();
}

shared_ptr<Component> ComponentFactory::Create(const std::string& ClassName){
	return _Create(("class " + ClassName));

}

std::unordered_map<std::string, std::function<shared_ptr<Component>()>>& ComponentFactory::GetComponents(){
	return mFactoryComponents;
}

shared_ptr<Component> ComponentFactory::_Create(std::string ClassName){
	auto p = mFactoryComponents.find(ClassName);
	if (p != mFactoryComponents.end()){
		return p->second();
	}
	return NULL;
}
