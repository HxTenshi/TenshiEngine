#include "NxActorFactory.h"
#include <fstream>


NxActorFactory::NxActorFactory(NxScene* scene):
mScene(scene){

}
void NxActorFactory::setScene(NxScene* scene){
	mScene = scene;
}
void NxActorFactory::registerPlan(const std::string& name, const std::string& fileName){
	NxActorCreator actorCreator(mScene);
	actorCreator.setPlan(std::ifstream(fileName.c_str()));
	registerProductCreator(name, actorCreator);
}