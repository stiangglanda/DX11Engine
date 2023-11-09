#pragma once
#include <PxPhysicsAPI.h>

using namespace physx;


class Physics
{
public:
	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation* gFoundation = NULL;
	PxPhysics* gPhysics = NULL;

	PxDefaultCpuDispatcher* gDispatcher = NULL;
	PxScene* gScene = NULL;

	PxMaterial* gMaterial = NULL;

	PxPvd* gPvd = NULL;

	PxReal stackZ = 10.0f;

	PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity = PxVec3(0));

	void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent);

	void initPhysics();

	void stepPhysics();

	void cleanupPhysics();
};

