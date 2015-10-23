#ifndef _NXACTORCREATOR_H_
#define _NXACTORCREATOR_H_

#include <NxPhysics.h>
#include <vector>
#include <iosfwd>
#include <memory>

class NxConvexMeshFolder;

class NxActorCreator{
public:
	NxActorCreator(NxScene* scene = 0);
	void initialize();
	void setPlan(std::istream& is);
	NxActor* operator() () const;

private:
	typedef std::shared_ptr<NxShapeDesc> NxShapeDescPtr;
	typedef std::shared_ptr<NxBodyDesc> NxBodyDescPtr;
	typedef std::vector<NxShapeDescPtr> NxShapeDescContainer;
	typedef std::shared_ptr<NxConvexMeshFolder> NxConvexMeshPtr;
	typedef std::vector<NxConvexMeshPtr> NxConvexMeshContainer;
	
	NxScene*				mScene;
	NxActorDesc				mActorDesc;
	NxShapeDescContainer	mActorShapesDescContainer;
	NxBodyDescPtr			mActorBodyDesc;
	NxConvexMeshContainer	mConvexMeshContainer;
};

#endif