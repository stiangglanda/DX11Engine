#include "SkinnedMesh.h"
#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4
#pragma pack_matrix( column_major )

SkinnedMesh::SkinnedMesh()
{
	m_pScene = nullptr;
}


SkinnedMesh::~SkinnedMesh()
{
	Clear();
}


void SkinnedMesh::Clear()
{
}

void SkinnedMesh::processNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		AssimpMesh mg = processMesh(mesh, scene);
		m_Entries.push_back(mg);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}
}

AssimpMesh SkinnedMesh::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex::PosNormalTexTanSkinned> vertices;
	std::vector<UINT> indices;
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;
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

	LoadBones(m_Entries.size(), mesh, vertices);

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

	return AssimpMesh(device, vertices, indices, DiffuseMapSRV, NormalMapSRV, mat);
}

void SkinnedMesh::LoadBones(int MeshIndex, const aiMesh* pMesh, std::vector<Vertex::PosNormalTexTanSkinned>& Bones)
{
	for (int i = 0; i < pMesh->mNumBones; i++)
	{
		int BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (boneMapping.find(BoneName) == boneMapping.end())
		{
			BoneIndex = bonesCount;
			bonesCount++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
		}
		else
		{
			BoneIndex = boneMapping[BoneName];
		}

		boneMapping[BoneName] = BoneIndex;
		m_BoneInfo[BoneIndex].BoneOffset =
			XMMatrixTranspose(static_cast<XMMATRIX>(&pMesh->mBones[i]->mOffsetMatrix.a1)); //probobly bullshit

		for (int j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
		{
			int VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			AddBoneData(BoneIndex, Weight, &Bones[VertexID]);
		}
	}
}

void SkinnedMesh::AddBoneData(int BoneID, float Weight, Vertex::PosNormalTexTanSkinned* bones)
{
	float weights[4];
	weights[0] = bones->Weights.x;
	weights[1] = bones->Weights.y;
	weights[2] = bones->Weights.z;
	weights[3] = 0;
	for (int i = 0; i < 4; i++)
	{
		//change if you want more bones
		if (weights[i] == 0)
		{
			bones->BoneIndices[i] = BoneID;
			weights[i] = Weight;
			bones->Weights.x = weights[0];
			bones->Weights.y = weights[1];
			bones->Weights.z = weights[2];
			return;
		}
	}

	// should never get here - more bones than we have space for
	assert(0);
}

bool SkinnedMesh::LoadMesh(ID3D11Device* device, ID3D11DeviceContext* md3dImmediateContext, TextureMgr* texMgr,
                           std::string Filename, std::wstring directory)
{
	// Release the previously loaded mesh (if it exists)
	Clear();

	bool Ret = false;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate |
	                               aiProcess_JoinIdenticalVertices |
	                               aiProcess_ConvertToLeftHanded |
	                               aiProcess_GenNormals |
	                               aiProcess_CalcTangentSpace |
	                               aiProcess_LimitBoneWeights);

	if (m_pScene)
	{
		this->directory = directory;
		this->Filename = Filename;
		this->texMgr = texMgr;

		this->device = device;
		this->md3dImmediateContext = md3dImmediateContext;


		m_GlobalInverseTransform = static_cast<XMMATRIX>(&m_pScene->mRootNode->mTransformation.Inverse().a1);
		m_GlobalInverseTransform = XMMatrixTranspose(m_GlobalInverseTransform);
		processNode(m_pScene->mRootNode, m_pScene);
	}
	else
	{
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	}

	return Ret;
}


void SkinnedMesh::Render(ID3D11DeviceContext* md3dImmediateContext, ID3DX11EffectTechnique* tech, bool tex)
{
	for (size_t i = 0; i < m_Entries.size(); ++i)
	{
		m_Entries[i].Draw(md3dImmediateContext, tech, tex);
	}
}


unsigned int SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < static_cast<float>(pNodeAnim->mPositionKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);

	return 0;
}


unsigned int SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < static_cast<float>(pNodeAnim->mRotationKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);

	return 0;
}


unsigned int SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < static_cast<float>(pNodeAnim->mScalingKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);

	return 0;
}


void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
	float Factor = (AnimationTime - static_cast<float>(pNodeAnim->mPositionKeys[PositionIndex].mTime)) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (AnimationTime - static_cast<float>(pNodeAnim->mRotationKeys[RotationIndex].mTime)) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	float Factor = (AnimationTime - static_cast<float>(pNodeAnim->mScalingKeys[ScalingIndex].mTime)) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const XMMATRIX ParentTransform)
{
	string NodeName(pNode->mName.data);
	const aiAnimation* pAnim = m_pScene->mAnimations[0];

	XMMATRIX NodeTransformation = XMMatrixTranspose(XMMATRIX(&pNode->mTransformation.a1));

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnim, NodeName);

	XMMATRIX anim = XMMatrixIdentity();
	if (pNodeAnim)
	{
		aiVector3D s;
		CalcInterpolatedScaling(s, AnimationTime, pNodeAnim);
		XMMATRIX ScalingM = XMMatrixScaling(s.x, s.y, s.z);

		aiQuaternion q;
		CalcInterpolatedRotation(q, AnimationTime, pNodeAnim);

		XMMATRIX RotationM = XMMatrixRotationQuaternion(XMVectorSet(q.x, q.y, q.z, q.w));

		RotationM = XMMatrixTranspose(RotationM);

		aiVector3D t;
		CalcInterpolatedPosition(t, AnimationTime, pNodeAnim);
		XMMATRIX TranslationM = XMMatrixTranslation(t.x, t.y, t.z);

		XMFLOAT4X4 mat;
		XMStoreFloat4x4(&mat, RotationM);

		mat._11 *= s.x;
		mat._21 *= s.x;
		mat._31 *= s.x;
		mat._12 *= s.y;
		mat._22 *= s.y;
		mat._32 *= s.y;
		mat._13 *= s.z;
		mat._23 *= s.z;
		mat._33 *= s.z;
		mat._14 = t.x;
		mat._24 = t.y;
		mat._34 = t.z;

		NodeTransformation = XMLoadFloat4x4(&mat);

		NodeTransformation = XMMatrixTranspose(NodeTransformation);
		//I applied transpos
	}

	XMMATRIX GlobalTransformation = NodeTransformation;
	GlobalTransformation *= ParentTransform;


	if (boneMapping.find(NodeName) != boneMapping.end())
	{
		int BoneIndex = boneMapping[NodeName];

		m_BoneInfo[BoneIndex].FinalTransformation = m_BoneInfo[BoneIndex].BoneOffset * GlobalTransformation *
			XMMatrixRotationZ(XM_PI);
	}

	for (UINT i = 0; i < pNode->mNumChildren; ++i)
	{
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

void SkinnedMesh::BoneTransform(float TimeInSeconds, vector<XMMATRIX>& Transforms)
{
	XMMATRIX Identity = XMMatrixIdentity();
	float TicksPerSecond = static_cast<float>(m_pScene->mAnimations[0]->mTicksPerSecond != 0
		                                          ? m_pScene->mAnimations[0]->mTicksPerSecond
		                                          : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, static_cast<float>(m_pScene->mAnimations[0]->mDuration));


	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

	Transforms.resize(bonesCount);

	for (unsigned int i = 0; i < bonesCount; i++)
	{
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++)
	{
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName)
		{
			return pNodeAnim;
		}
	}

	return nullptr;
}
