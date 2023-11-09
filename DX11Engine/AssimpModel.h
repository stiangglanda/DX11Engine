#pragma once
#include "TextureMgr.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include "AssimpMesh.h"
#include "SkinnedData.h"

struct BoneInfo
{
	XMFLOAT4X4 BoneOffset;
	XMFLOAT4X4 FinalTransformation;
};


class AssimpModel
{
public:
	AssimpModel();
	~AssimpModel();

	bool Load(ID3D11Device* device, ID3D11DeviceContext* md3dImmediateContext, TextureMgr* texMgr, std::string filename,
	          std::wstring dirictory);
	void Draw(ID3D11DeviceContext* md3dImmediateContext, ID3DX11EffectTechnique* tech, bool tex);
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<AssimpMesh> meshes;
	std::map<std::string, unsigned int> boneMapping;
	std::vector<BoneInfo> boneMatrices;
	unsigned int bonesCount = 0;
	SkinnedData SkinDat;
	void Close();

private:
	ID3D11Device* device;
	ID3D11DeviceContext* md3dImmediateContext;

	std::wstring directory;
	std::string filename;
	std::vector<Material> Mat;

	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;
	TextureMgr* texMgr;

	void processNode(aiNode* node, const aiScene* scene);
	AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<ID3D11ShaderResourceView*> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
	                                                            std::string typeName, const aiScene* scene);
};


struct AssimpModelInstance
{
	AssimpModel* Model;
	XMFLOAT4X4 World;
};
