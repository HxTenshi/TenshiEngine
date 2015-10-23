#ifndef _NXACTORFACTORY_H_
#define _NXACTORFACTORY_H_

#include "Factory.h"
#include "NxActorCreator.h"

class NxActorFactory : public Factory<NxActor, std::string, NxActorCreator>{
public:
	NxActorFactory(NxScene* scene = 0);
	void setScene(NxScene* scene);
	void registerPlan(const std::string& name, const std::string& fileName);

private:
	NxScene* mScene;
};


#endif