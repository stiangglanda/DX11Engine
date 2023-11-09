#pragma once
#include <string>
#include <DirectXMath.h>
#include "SkinnedMesh.h"
#include "AssimpModel.h"

namespace component
{
	static const char* components[] = {"Name", "Shape", "Transform", "Static Model", "Physics", "Skeletal Model"};


	struct Name
	{
		std::string name;
	};

	struct Shape
	{
		ID3D11Buffer* ShapeVB;
		ID3D11Buffer* ShapeIB;
	};

	struct Transform
	{
		XMFLOAT4X4 Transform;
	};

	struct StaticModel
	{
		AssimpModel model;
	};

	struct SkeletalModel
	{
		SkinnedMesh skelmodel;
	};

	struct RigidPhysics
	{
		int physicsIndex;
		bool enablePhx;
	};
}
