#include "AssimpModel.h"
#include <assimp/postprocess.h>
#include "MeshGeometry.h"
#include "SkinnedData.h"

AssimpModel::AssimpModel() :
	device(nullptr),
	md3dImmediateContext(nullptr)
{
	// empty
}


AssimpModel::~AssimpModel()
{
	// empty
}

bool AssimpModel::Load(ID3D11Device* device, ID3D11DeviceContext* md3dImmediateContext, TextureMgr* texMgr,
                       std::string filename, std::wstring directory)
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(filename.c_str(),
	                                      aiProcess_Triangulate |
	                                      aiProcess_JoinIdenticalVertices |
	                                      aiProcess_ConvertToLeftHanded |
	                                      aiProcess_GenNormals |
	                                      aiProcess_CalcTangentSpace |
	                                      aiProcess_LimitBoneWeights
	);

	if (pScene == nullptr)
		return false;

	this->directory = directory;
	this->filename = filename;
	this->texMgr = texMgr;

	this->device = device;
	this->md3dImmediateContext = md3dImmediateContext;

	processNode(pScene->mRootNode, pScene);

	return true;
}

void AssimpModel::Draw(ID3D11DeviceContext* md3dImmediateContext, ID3DX11EffectTechnique* tech, bool tex)
{
	for (size_t i = 0; i < meshes.size(); ++i)
	{
		meshes[i].Draw(md3dImmediateContext, tech, tex);
	}
}

AssimpMesh AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex::PosNormalTexTanSkinned> vertices;
	std::vector<UINT> indices;
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;
	std::vector<MeshGeometry::Subset> subsets;
	std::vector<Material> mat;

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex::PosNormalTexTanSkinned vertex;

		vertex.Pos.x = mesh->mVertices[i].x;
		vertex.Pos.y = mesh->mVertices[i].y;
		vertex.Pos.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.Tex.x = mesh->mTextureCoords[0][i].x;
			vertex.Tex.y = mesh->mTextureCoords[0][i].y;
		}

		if (mesh->mNormals)
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}

		if (mesh->mTangents)
		{
			vertex.TangentU.x = mesh->mTangents[i].x;
			vertex.TangentU.y = mesh->mTangents[i].y;
			vertex.TangentU.z = mesh->mTangents[i].z;
			vertex.TangentU.w = 1;
		}
		vertex.Weights.x = 0;
		vertex.Weights.y = 0;
		vertex.Weights.z = 0;

		vertices.push_back(vertex);
	}


	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		unsigned int boneIndex = 0;
		std::string boneName(mesh->mBones[i]->mName.data);

		if (boneMapping.find(boneName) == boneMapping.end())
		{
			// allocate an index for the new bone
			boneIndex = bonesCount;
			bonesCount++;
			BoneInfo boneMatrix;
			boneMatrices.push_back(boneMatrix);
			boneMatrices[boneIndex].BoneOffset._11 = mesh->mBones[i]->mOffsetMatrix.a1;
			boneMatrices[boneIndex].BoneOffset._12 = mesh->mBones[i]->mOffsetMatrix.a2;
			boneMatrices[boneIndex].BoneOffset._13 = mesh->mBones[i]->mOffsetMatrix.a3;
			boneMatrices[boneIndex].BoneOffset._14 = mesh->mBones[i]->mOffsetMatrix.a4;
			boneMatrices[boneIndex].BoneOffset._21 = mesh->mBones[i]->mOffsetMatrix.b1;
			boneMatrices[boneIndex].BoneOffset._22 = mesh->mBones[i]->mOffsetMatrix.b2;
			boneMatrices[boneIndex].BoneOffset._23 = mesh->mBones[i]->mOffsetMatrix.b3;
			boneMatrices[boneIndex].BoneOffset._24 = mesh->mBones[i]->mOffsetMatrix.b4;
			boneMatrices[boneIndex].BoneOffset._31 = mesh->mBones[i]->mOffsetMatrix.c1;
			boneMatrices[boneIndex].BoneOffset._32 = mesh->mBones[i]->mOffsetMatrix.c2;
			boneMatrices[boneIndex].BoneOffset._33 = mesh->mBones[i]->mOffsetMatrix.c3;
			boneMatrices[boneIndex].BoneOffset._34 = mesh->mBones[i]->mOffsetMatrix.c4;
			boneMatrices[boneIndex].BoneOffset._41 = mesh->mBones[i]->mOffsetMatrix.d1;
			boneMatrices[boneIndex].BoneOffset._42 = mesh->mBones[i]->mOffsetMatrix.d2;
			boneMatrices[boneIndex].BoneOffset._43 = mesh->mBones[i]->mOffsetMatrix.d3;
			boneMatrices[boneIndex].BoneOffset._44 = mesh->mBones[i]->mOffsetMatrix.d4;
			boneMapping[boneName] = boneIndex;
		}
		else
			boneIndex = boneMapping[boneName];

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;

			for (unsigned int g = 0; g < 4; g++)
			{
				float* weights = nullptr;
				float tmp = 0;
				switch (g)
				{
				case 0:
					{
						weights = &vertices[vertexID].Weights.x;
						break;
					}
				case 1:
					{
						weights = &vertices[vertexID].Weights.y;
						break;
					}
				case 2:
					{
						weights = &vertices[vertexID].Weights.z;
						break;
					}
				case 3:
					{
						weights = &tmp;
						break;
					}
				default:
					break;
				}
				if (*weights == 0.0)
				{
					vertices[vertexID].BoneIndices[g] = static_cast<BYTE>(boneIndex);
					if (g != 3)
					{
						*weights = weight;
					}
					break;
				}
			}
		}
	}

	for (int i = 0; i < vertices.size(); i++)
	{
		float totalWeight = vertices[i].Weights.x + vertices[i].Weights.y + vertices[i].Weights.z;
		if (totalWeight > 0.0f)
		{
			vertices[i].Weights.x = vertices[i].Weights.x / totalWeight;
			vertices[i].Weights.y = vertices[i].Weights.y / totalWeight;
			vertices[i].Weights.z = vertices[i].Weights.z / totalWeight;
		}
	}

	std::vector<ID3D11ShaderResourceView*> materials;
	materials.reserve(scene->mNumMaterials);
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	aiString texFileName;
	if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
	{
		std::string filename = texFileName.C_Str();
		std::wstring filename2(filename.begin(), filename.end());

		std::wstring wdirectory(directory.begin(), directory.end());

		ID3D11ShaderResourceView* diffuseMapSRV = texMgr->CreateWICTexture(wdirectory + filename2);
		DiffuseMapSRV.push_back(diffuseMapSRV);
		DiffuseMapSRV.push_back(diffuseMapSRV);
	}

	if (material->GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
	{
		std::string filename = texFileName.C_Str();
		std::wstring filename2(filename.begin(), filename.end());

		std::wstring wdirectory(directory.begin(), directory.end());

		ID3D11ShaderResourceView* normalMapSRV = texMgr->CreateWICTexture(wdirectory + filename2);
		DiffuseMapSRV.push_back(normalMapSRV);
		NormalMapSRV.push_back(normalMapSRV);
	}

	Material Mat;
	material->Get(AI_MATKEY_COLOR_AMBIENT, Mat.Ambient);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, Mat.Diffuse);
	material->Get(AI_MATKEY_COLOR_REFLECTIVE, Mat.Reflect);
	material->Get(AI_MATKEY_COLOR_SPECULAR, Mat.Specular);
	mat.push_back(Mat);

	SkinnedData skinInfo;

	return AssimpMesh(device, vertices, indices, DiffuseMapSRV, NormalMapSRV, mat);
}

const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const std::string nodeName)
{
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		if (std::string(nodeAnim->mNodeName.data) == nodeName)
			return nodeAnim;
	}

	return nullptr;
}

std::vector<ID3D11ShaderResourceView*> AssimpModel::loadMaterialTextures(
	aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene)
{
	std::vector<ID3D11ShaderResourceView*> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		std::string filename = str.C_Str();
		std::wstring filename2(filename.begin(), filename.end());

		std::wstring wdirectory(directory.begin(), directory.end());


		ID3D11ShaderResourceView* diffuseMapSRV = texMgr->CreateWICTexture(wdirectory + filename2);
		DiffuseMapSRV.push_back(diffuseMapSRV);
	}
	return textures;
}

void AssimpModel::Close()
{
}

void AssimpModel::processNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		AssimpMesh mg = processMesh(mesh, scene);
		meshes.push_back(mg);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}
