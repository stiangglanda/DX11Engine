
#pragma once
#include <string>
#include <DirectXMath.h>
#include "SkinnedMesh.h"
#include "AssimpModel.h"

namespace component
{
    static const char* components[] = { "Name","Shape","Transform","SkinnedMesh","Physics" };


    struct Name
    {
        std::string name;
    };

    struct Transform
    {
        DirectX::XMFLOAT4X4 Transform;
    };

    struct Shape
    {
        ID3D11Buffer* ShapeVB;
        ID3D11Buffer* ShapeIB;
    }; 
    
    struct Cskinnedmesh
    {
        std::string skinnedMesh;
    };

    struct RigidPhysics
    {
        int physicsIndex;
        bool enablePhx;
    };
}