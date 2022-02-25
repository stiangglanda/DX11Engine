#include "AssimpModel.h"
#include "MeshGeometry.h"
#include "SkinnedData.h"

AssimpModel::AssimpModel() :
	device(nullptr),
	md3dImmediateContext(nullptr),
	meshes(),
	Mat(),
	DiffuseMapSRV(),
	NormalMapSRV(),
	directory() {
	// empty
}


AssimpModel::~AssimpModel() {
	// empty
}

bool AssimpModel::Load(ID3D11Device* device, ID3D11DeviceContext* md3dImmediateContext, TextureMgr* texMgr, std::string filename,std::wstring directory) {

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

void AssimpModel::Draw(ID3D11DeviceContext* md3dImmediateContext, ID3DX11EffectTechnique* tech, bool tex) {
	for (size_t i = 0; i < meshes.size(); ++i) {
		meshes[i].Draw(md3dImmediateContext, tech,tex);
	}
}

AssimpMesh AssimpModel::processMesh(aiMesh* mesh, const aiScene* scene) {
	// Data to fill
	std::vector<Vertex::PosNormalTexTanSkinned> vertices;
	std::vector<UINT> indices;
	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;
	std::vector<MeshGeometry::Subset> subsets;
	std::vector <Material> mat;
	//std::vector<int> boneHierarchy(mesh->mNumBones);
	//std::vector<XMFLOAT4X4> boneOffsets;
	//std::map<std::string, AnimationClip> animations;

	
	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		Vertex::PosNormalTexTanSkinned vertex;

		vertex.Pos.x = mesh->mVertices[i].x;
		vertex.Pos.y = mesh->mVertices[i].y;
		vertex.Pos.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0]) {
			vertex.Tex.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.Tex.y = (float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->mNormals)
		{
			vertex.Normal.x = (float)mesh->mNormals[i].x;
			vertex.Normal.y = (float)mesh->mNormals[i].y;
			vertex.Normal.z = (float)mesh->mNormals[i].z;
		}

		if (mesh->mTangents)
		{
			vertex.TangentU.x = (float)mesh->mTangents[i].x;
			vertex.TangentU.y = (float)mesh->mTangents[i].y;
			vertex.TangentU.z = (float)mesh->mTangents[i].z;
			vertex.TangentU.w = 1;
		}
		vertex.Weights.x = 0;
		vertex.Weights.y = 0;
		vertex.Weights.z = 0;
		
		/*vertex.BoneIndices;
		vertex.Weights.x
		vertex.Weights.y
		vertex.Weights.z*/

		vertices.push_back(vertex);
	}


	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//std::map<int, int> boneMap0;
	//std::map<int, int> boneMap1;

	//for (size_t i = 0; i < mesh->mNumBones; i++)
	//{
	//	auto bone = mesh->mBones[i];
	//	for (size_t j = 0; j < bone->mNumWeights; j++)
	//	{
	//		if (!(boneMap0.find(bone->mWeights[j].mVertexId) != boneMap0.end()))
	//		{

	//		}
	//		else if(boneMap0[bone->mWeights[j].mVertexId])
	//		{

	//		}
	//	}
	//}

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
		/*	boneHierarchy.push_back(boneIndex);
			boneOffsets.push_back(boneMatrices[boneIndex].BoneOffset);*/
		}
		else
			boneIndex = boneMapping[boneName];

		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;

			for (unsigned int g = 0; g < 4; g++)
			{
				float* weights=nullptr;
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
					vertices[vertexID].BoneIndices[g] = (BYTE)boneIndex;
					if (g != 3)
					{
						*weights = weight;
					}
					break;
				}
			}
		}
	}

	for (int i = 0; i < vertices.size(); i++) {
		float totalWeight = vertices[i].Weights.x + vertices[i].Weights.y + vertices[i].Weights.z;
		if (totalWeight > 0.0f) {
			vertices[i].Weights.x = vertices[i].Weights.x / totalWeight;
			vertices[i].Weights.y = vertices[i].Weights.y / totalWeight;
			vertices[i].Weights.z = vertices[i].Weights.z / totalWeight;
		}
	}

	//MeshGeometry::Subset mgs;
	//mgs.FaceCount = mesh->mNumFaces;
	//mgs.Id = 0;
	//mgs.VertexCount = mesh->mNumVertices;
	//mgs.VertexStart = 0;
	//mgs.FaceStart = 0;
	//subsets.push_back(mgs);

	//if (mesh->mMaterialIndex >= 0) {
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	//	std::vector<ID3D11ShaderResourceView*> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//}

	std::vector<ID3D11ShaderResourceView*> materials;
	materials.reserve(scene->mNumMaterials);
	/*for (size_t i = 0; i < scene->mNumMaterials; i++)
	{*/
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//for (size_t i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
		//{
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
		//}
	//}
	SkinnedData skinInfo;
	//skinInfo.Set(boneHierarchy, boneOffsets, animations);
	//mg.SetVertices(device, &vertices[0], vertices.size());
	//mg.SetIndices(device, &indices[0], indices.size());
	//mg.SetSubsetTable(subsets);
	return AssimpMesh(device, vertices, indices, DiffuseMapSRV, NormalMapSRV,mat);
}

//void readNodeHeirarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform)
//{
//	std::string nodeName(node->mName.data);
//
//	const aiAnimation* animation = scene->mAnimations[currentAnimation];
//
//	glm::mat4 nodeTransformation = mat4Convert(node->mTransformation);
//
//	const aiNodeAnim* nodeAnim = findNodeAnim(animation, nodeName);
//
//	if (nodeAnim)
//	{
//		// Interpolate scaling and generate scaling transformation matrix
//		aiVector3D scaling;
//		calcInterpolatedScaling(scaling, animationTime, nodeAnim);
//		glm::vec3 scale = glm::vec3(scaling.x, scaling.y, scaling.z);
//		glm::mat4 scalingM = glm::scale(glm::mat4(1.0f), scale);
//
//		// Interpolate rotation and generate rotation transformation matrix
//		aiQuaternion rotationQ;
//		calcInterpolatedRotation(rotationQ, animationTime, nodeAnim);
//		glm::quat rotate = quatConvert(rotationQ);
//		glm::mat4 rotationM = glm::toMat4(rotate);
//
//		// Interpolate translation and generate translation transformation matrix
//		aiVector3D translation;
//		calcInterpolatedPosition(translation, animationTime, nodeAnim);
//		glm::vec3 translate = glm::vec3(translation.x, translation.y, translation.z);
//		glm::mat4 translationM = glm::translate(glm::mat4(1.0f), translate);
//
//		// Combine the above transformations
//		nodeTransformation = translationM * rotationM * scalingM;
//	}
//
//	// Combine with node Transformation with Parent Transformation
//	glm::mat4 globalTransformation = parentTransform * nodeTransformation;
//
//	if (boneMapping.find(nodeName) != boneMapping.end())
//	{
//		unsigned int boneIndex = boneMapping[nodeName];
//		boneMatrices[boneIndex].FinalTransformation = globalInverseTransform * globalTransformation * boneMatrices[boneIndex].BoneOffset;
//	}
//
//	for (unsigned int i = 0; i < node->mNumChildren; i++)
//		readNodeHeirarchy(animationTime, node->mChildren[i], globalTransformation);
//}

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

std::vector<ID3D11ShaderResourceView*> AssimpModel::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
	std::vector<ID3D11ShaderResourceView*> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		std::string filename = str.C_Str();
		std::wstring filename2(filename.begin(),filename.end());

		std::wstring wdirectory(directory.begin(), directory.end());


		ID3D11ShaderResourceView* diffuseMapSRV = texMgr->CreateWICTexture(wdirectory + filename2);
		DiffuseMapSRV.push_back(diffuseMapSRV);
	}
	return textures;
}

void AssimpModel::Close() {
}

void AssimpModel::processNode(aiNode* node, const aiScene* scene) {
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		AssimpMesh mg=processMesh(mesh, scene);
		meshes.push_back(mg);
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}