#pragma once
#include <DirectXMath.h>
#include <TextureMgr.h>
#include <assimp/postprocess.h>  // Post processing flags
#include <assimp/scene.h>        // Output data structure

#include <assimp/Importer.hpp>  // C++ importer interface
#include <map>
#include <vector>

#include "AssimpMesh.h"
#pragma pack_matrix(column_major)
using namespace DirectX;

using namespace std;

class SkinnedMesh
{
public:
	struct BoneInfo
	{
		XMMATRIX BoneOffset;
		XMMATRIX FinalTransformation;
	};


	SkinnedMesh();
	~SkinnedMesh();
	TextureMgr* texMgr;
	ID3D11Device* device;
	ID3D11DeviceContext* md3dImmediateContext;
	std::string Filename;
	unsigned int bonesCount = 0;
	std::map<std::string, unsigned int> boneMapping;
	std::wstring directory;
	bool LoadMesh(ID3D11Device* device, ID3D11DeviceContext* md3dImmediateContext,
	              TextureMgr* texMgr, std::string Filename,
	              std::wstring directory);
	void processNode(aiNode* node, const aiScene* scene);
	AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene);
	void Render(ID3D11DeviceContext* md3dImmediateContext,
	            ID3DX11EffectTechnique* tech, bool tex);
	void LoadBones(int MeshIndex, const aiMesh* pMesh,
	               std::vector<Vertex::PosNormalTexTanSkinned>& Bones);
	void AddBoneData(int BoneID, float Weight,
	                 Vertex::PosNormalTexTanSkinned* bones);

	unsigned int NumBones() const { return bonesCount; }

	void BoneTransform(float TimeInSeconds, vector<XMMATRIX>& Transforms);
	vector<AssimpMesh> m_Entries;

private:
#define NUM_BONES_PER_VEREX 4
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime,
	                             const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime,
	                              const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime,
	                              const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation,
	                               string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode,
	                       XMMATRIX ParentTransform);
	void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

	vector<ID3D11ShaderResourceView*> m_Textures;

	vector<BoneInfo> m_BoneInfo;
	XMMATRIX m_GlobalInverseTransform;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};
