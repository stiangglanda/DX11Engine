#include "SkinnedMesh.h"
#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4
#pragma pack_matrix( column_major )

//void SkinnedMesh::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
//{
//    for (unsigned int i = 0; i < (sizeof(IDs) / sizeof(IDs[0])); i++) {
//        if (Weights[i] == 0.0) {
//            IDs[i] = BoneID;
//            Weights[i] = Weight;
//            return;
//        }
//    }
//
//    // should never get here - more bones than we have space for
//    assert(0);
//}

SkinnedMesh::SkinnedMesh()
{
    //ZERO_MEM(m_Buffers);
    m_pScene = NULL;
}


SkinnedMesh::~SkinnedMesh()
{
    Clear();
}


void SkinnedMesh::Clear()
{
 /*   for (unsigned int i = 0; i < m_Textures.size(); i++) {
        SAFE_DELETE(m_Textures[i]);
    }

    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }

    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }*/
}

void SkinnedMesh::processNode(aiNode* node, const aiScene* scene) {
    for (UINT i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        AssimpMesh mg = processMesh(mesh, scene);
        m_Entries.push_back(mg);
    }

    for (UINT i = 0; i < node->mNumChildren; i++) {
        this->processNode(node->mChildren[i], scene);
    }
}

AssimpMesh SkinnedMesh::processMesh(aiMesh* mesh, const aiScene* scene) {
    // Data to fill
    std::vector<Vertex::PosNormalTexTanSkinned> vertices;
    std::vector<UINT> indices;
    std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
    std::vector<ID3D11ShaderResourceView*> NormalMapSRV;
    std::vector <Material> mat;
    //std::vector<int> boneHierarchy(mesh->mNumBones);
    //std::vector<XMFLOAT4X4> boneOffsets;
    //std::map<std::string, AnimationClip> animations;


    // Walk through each of the mesh's vertices
    for (UINT i = 0; i < mesh->mNumVertices; i++) {
        Vertex::PosNormalTexTanSkinned vertex;
        vertex.Pos.x = mesh->mVertices[i].x;
        vertex.Pos.y = mesh->mVertices[i].y ;
        vertex.Pos.z = mesh->mVertices[i].z ;

        if (mesh->mTextureCoords[0]) {
            vertex.Tex.x = (float)mesh->mTextureCoords[0][i].x;
            vertex.Tex.y = (float)mesh->mTextureCoords[0][i].y;
        }

        if (mesh->mNormals)
        {
            vertex.Normal.x = (float)mesh->mNormals[i].x ;
            vertex.Normal.y = (float)mesh->mNormals[i].y ;
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
  /*      vertex.BoneIndices[0]= -1;
        vertex.BoneIndices[1]= -1;
        vertex.BoneIndices[2]= -1;
        vertex.BoneIndices[3]=-1;*/

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

    LoadBones(m_Entries.size(), mesh, vertices);
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

    //for (unsigned int i = 0; i < mesh->mNumBones; i++)
    //{
    //    unsigned int boneIndex = 0;
    //    std::string boneName(mesh->mBones[i]->mName.data);

    //    if (boneMapping.find(boneName) == boneMapping.end())
    //    {
    //        // allocate an index for the new bone
    //        boneIndex = bonesCount;
    //        bonesCount++;
    //        BoneInfo boneMatrix;
    //        XMFLOAT4X4 bo;
    //        
    //        bo._11 = mesh->mBones[i]->mOffsetMatrix.a1;
    //        bo._12 = mesh->mBones[i]->mOffsetMatrix.a2;
    //        bo._13 = mesh->mBones[i]->mOffsetMatrix.a3;
    //        bo._14 = mesh->mBones[i]->mOffsetMatrix.a4;
    //        bo._21 = mesh->mBones[i]->mOffsetMatrix.b1;
    //        bo._22 = mesh->mBones[i]->mOffsetMatrix.b2;
    //        bo._23 = mesh->mBones[i]->mOffsetMatrix.b3;
    //        bo._24 = mesh->mBones[i]->mOffsetMatrix.b4;
    //        bo._31 = mesh->mBones[i]->mOffsetMatrix.c1;
    //        bo._32 = mesh->mBones[i]->mOffsetMatrix.c2;
    //        bo._33 = mesh->mBones[i]->mOffsetMatrix.c3;
    //        bo._34 = mesh->mBones[i]->mOffsetMatrix.c4;
    //        bo._41 = mesh->mBones[i]->mOffsetMatrix.d1;
    //        bo._42 = mesh->mBones[i]->mOffsetMatrix.d2;
    //        bo._43 = mesh->mBones[i]->mOffsetMatrix.d3;
    //        bo._44 = mesh->mBones[i]->mOffsetMatrix.d4;
    //        boneMatrix.BoneOffset = XMLoadFloat4x4(&bo);
    //        m_BoneInfo.push_back(boneMatrix);
    //        boneMapping[boneName] = boneIndex;
    //        /*	boneHierarchy.push_back(boneIndex);
    //            boneOffsets.push_back(boneMatrices[boneIndex].BoneOffset);*/
    //    }
    //    else
    //        boneIndex = boneMapping[boneName];

    //    for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
    //    {
    //        unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
    //        float weight = mesh->mBones[i]->mWeights[j].mWeight;

    //        for (unsigned int g = 0; g < 4; g++)
    //        {
    //            float* weights = nullptr;
    //            float tmp = 0;
    //            switch (g)
    //            {
    //            case 0:
    //            {
    //                weights = &vertices[vertexID].Weights.x;
    //                break;
    //            }
    //            case 1:
    //            {
    //                weights = &vertices[vertexID].Weights.y;
    //                weights = &tmp;
    //                break;
    //            }
    //            case 2:
    //            {
    //                weights = &vertices[vertexID].Weights.z;
    //                break;
    //            }
    //            case 3:
    //            {
    //                weights = &tmp;
    //                break;
    //            }
    //            default:
    //                break;
    //            }
    //            if (*weights == 0.0)
    //            {
    //                vertices[vertexID].BoneIndices[g] = (BYTE)boneIndex;
    //                if (g != 3)
    //                {
    //                    *weights = weight;
    //                }
    //                break;
    //            }
    //        }
    //    }
    //}

    //for (int i = 0; i < vertices.size(); i++) {
    //    float totalWeight = vertices[i].Weights.x + vertices[i].Weights.y + vertices[i].Weights.z;
    //    if (totalWeight > 0.0f) {
    //        vertices[i].Weights.x = vertices[i].Weights.x / totalWeight;
    //        vertices[i].Weights.y = vertices[i].Weights.y / totalWeight;
    //        vertices[i].Weights.z = vertices[i].Weights.z / totalWeight;
    //    }
    //}

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
    //skinInfo.Set(boneHierarchy, boneOffsets, animations);
    //mg.SetVertices(device, &vertices[0], vertices.size());
    //mg.SetIndices(device, &indices[0], indices.size());
    //mg.SetSubsetTable(subsets);
    return AssimpMesh(device, vertices, indices, DiffuseMapSRV, NormalMapSRV, mat);
}

void SkinnedMesh::LoadBones(int MeshIndex, const aiMesh* pMesh, std::vector<Vertex::PosNormalTexTanSkinned>& Bones)
{
    for (int i = 0; i < pMesh->mNumBones; i++) {
        int BoneIndex = 0;
        string BoneName(pMesh->mBones[i]->mName.data);

        if (boneMapping.find(BoneName) == boneMapping.end()) {
            BoneIndex = bonesCount;
            bonesCount++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
        }
        else {
            BoneIndex = boneMapping[BoneName];
        }

        boneMapping[BoneName] = BoneIndex;
        m_BoneInfo[BoneIndex].BoneOffset = XMMatrixTranspose( (XMMATRIX)&pMesh->mBones[i]->mOffsetMatrix.a1);//probobly bullshit
        //m_BoneInfo[BoneIndex].BoneOffset =  (XMMATRIX)&pMesh->mBones[i]->mOffsetMatrix.a1;//probobly bullshit
        //m_BoneInfo[BoneIndex].BoneOffset =
        //XMMATRIX(pMesh->mBones[i]->mOffsetMatrix.a1, pMesh->mBones[i]->mOffsetMatrix.a2, pMesh->mBones[i]->mOffsetMatrix.a3, pMesh->mBones[i]->mOffsetMatrix.a4,
        //    pMesh->mBones[i]->mOffsetMatrix.b1, pMesh->mBones[i]->mOffsetMatrix.b2, pMesh->mBones[i]->mOffsetMatrix.b3, pMesh->mBones[i]->mOffsetMatrix.b4,
        //    pMesh->mBones[i]->mOffsetMatrix.c1, pMesh->mBones[i]->mOffsetMatrix.c2, pMesh->mBones[i]->mOffsetMatrix.c3, pMesh->mBones[i]->mOffsetMatrix.c4,
        //    pMesh->mBones[i]->mOffsetMatrix.d1, pMesh->mBones[i]->mOffsetMatrix.d2, pMesh->mBones[i]->mOffsetMatrix.d3, pMesh->mBones[i]->mOffsetMatrix.d4);

        for (int j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
            int VertexID =  pMesh->mBones[i]->mWeights[j].mVertexId;//m_Entries[MeshIndex].BaseVertex or 0 or pMesh->mNumVertices
            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
            AddBoneData(BoneIndex, Weight, &Bones[VertexID]);
        }
    }
}

void SkinnedMesh::AddBoneData(int BoneID, float Weight, Vertex::PosNormalTexTanSkinned* bones)
{
    float weights[4];
    weights[0]= bones->Weights.x;
    weights[1]= bones->Weights.y;
    weights[2]= bones->Weights.z;
    weights[3]= 0;
    for (int i = 0; i < 4; i++) {//change if you want more bones
        if (weights[i] == 0)
        {
            bones->BoneIndices[i] = BoneID;
            weights[i]= Weight;
            bones->Weights.x = weights[0];
            bones->Weights.y = weights[1];
            bones->Weights.z = weights[2];
            return;
        }
       /* if (*weights[i] == 0.0) {
            switch (i)
            {
            case 0:
            {
                bones->BoneIndices[0] = BoneID;
                bones->Weights.x = Weight;
                break;
            }
            case 1:
            {
                bones->BoneIndices[1] = BoneID;
                bones->Weights.y = Weight;
                break;
            }
            case 2:
            {
                bones->BoneIndices[2] = BoneID;
                bones->Weights.z = Weight;
                break;
            }
            case 3:
            {
                bones->BoneIndices[3] = BoneID;
                
                break;
            }
            default:
                break;
            }
            return;
        }*/
    }

    // should never get here - more bones than we have space for
    assert(0);
}

bool SkinnedMesh::LoadMesh(ID3D11Device* device, ID3D11DeviceContext* md3dImmediateContext, TextureMgr* texMgr, std::string Filename, std::wstring directory)
{
    // Release the previously loaded mesh (if it exists)
    Clear();

    // Create the VAO
    //glGenVertexArrays(1, &m_VAO);
    //glBindVertexArray(m_VAO);

    // Create the buffers for the vertices attributes
    //glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool Ret = false;

    m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_LimitBoneWeights);

    if (m_pScene) {

        this->directory = directory;
        this->Filename = Filename;
        this->texMgr = texMgr;

        this->device = device;
        this->md3dImmediateContext = md3dImmediateContext;

        
        m_GlobalInverseTransform = (XMMATRIX)&m_pScene->mRootNode->mTransformation.Inverse().a1;
        m_GlobalInverseTransform = XMMatrixTranspose(m_GlobalInverseTransform);
        //m_GlobalInverseTransform = XMMatrixIdentity();
        //m_GlobalInverseTransform = XMMatrixTranspose( (XMMATRIX)&m_pScene->mRootNode->mTransformation.Inverse().a1);
        //m_GlobalInverseTransform = (XMMATRIX)&m_pScene->mRootNode->mTransformation.a1;
        //m_GlobalInverseTransform.invers;
        //m_GlobalInverseTransform = MathHelper::Inverse(m_GlobalInverseTransform);
        processNode(m_pScene->mRootNode, m_pScene);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
    }

    // Make sure the VAO is not changed from the outside
    //glBindVertexArray(0);

    return Ret;
}




void SkinnedMesh::Render(ID3D11DeviceContext* md3dImmediateContext, ID3DX11EffectTechnique* tech, bool tex)
{
    //glBindVertexArray(m_VAO);
    for (size_t i = 0; i < m_Entries.size(); ++i) {
        m_Entries[i].Draw(md3dImmediateContext, tech, tex);
    }
    //for (unsigned int i = 0; i < m_Entries.size(); i++) {
    //    const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

    //    assert(MaterialIndex < m_Textures.size());

    //    if (m_Textures[MaterialIndex]) {
    //        m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
    //    }

    //    glDrawElementsBaseVertex(GL_TRIANGLES,
    //        m_Entries[i].NumIndices,
    //        GL_UNSIGNED_INT,
    //        (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex),
    //        m_Entries[i].BaseVertex);
    //}

    // Make sure the VAO is not changed from the outside    
    //glBindVertexArray(0);
}


unsigned int SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


unsigned int SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


unsigned int SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    unsigned int NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}


void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    unsigned int NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const XMMATRIX ParentTransform)
{

    //string NodeName(pNode->mName.data);

    //const aiAnimation* pAnimation = m_pScene->mAnimations[0];

    //XMMATRIX NodeTransformation= XMMATRIX(&pNode->mTransformation.a1);

    //const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    //if (pNodeAnim) {
    //    // Interpolate scaling and generate scaling transformation matrix
    //    aiVector3D Scaling;
    //    CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
    //    XMMATRIX ScalingM;
    //    ScalingM=XMMatrixScaling(Scaling.x, Scaling.y, Scaling.z);
    //    //ScalingM= XMMatrixTranspose(ScalingM);
    //    //ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

    //    // Interpolate rotation and generate rotation transformation matrix
    //    aiQuaternion RotationQ;
    //    CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
    //    XMMATRIX RotationM = XMMatrixRotationQuaternion(XMVectorSet(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w));
    //    //RotationM = XMMatrixTranspose(RotationM);

    //    // Interpolate translation and generate translation transformation matrix
    //    aiVector3D Translation;
    //    CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
    //    XMMATRIX TranslationM = XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
    //    //TranslationM = XMMatrixTranspose(TranslationM);

    //    // Combine the above transformations
    //    NodeTransformation = ScalingM* RotationM * TranslationM;
    //    NodeTransformation = XMMatrixTranspose(NodeTransformation);

    //}

    //XMMATRIX GlobalTransformation = ParentTransform * NodeTransformation;
    ////GlobalTransformation = XMMatrixTranspose(GlobalTransformation);

    //if (boneMapping.find(NodeName) != boneMapping.end()) {
    //    unsigned int BoneIndex = boneMapping[NodeName];

    //    m_BoneInfo[BoneIndex].FinalTransformation =
    //         //m_BoneInfo[BoneIndex].BoneOffset *GlobalTransformation * m_GlobalInverseTransform;
    //        m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    //}

    //for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
    //    ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    //}
    string NodeName(pNode->mName.data);
    const aiAnimation* pAnim = m_pScene->mAnimations[0];

    XMMATRIX NodeTransformation = XMMatrixTranspose(XMMATRIX(&pNode->mTransformation.a1));
    //XMMATRIX NodeTransformation = XMMatrixTranspose( XMMATRIX(&pNode->mTransformation.a1));
    //I just read aiMatrix4x4 (aiMatrix to XMMATRIX format)
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnim, NodeName);

    XMMATRIX anim = XMMatrixIdentity();
    if (pNodeAnim) {
        aiVector3D s;
        CalcInterpolatedScaling(s, AnimationTime, pNodeAnim);
        //XMMATRIX ScalingM = XMMatrixScaling(1, 1, 1);
        XMMATRIX ScalingM = XMMatrixScaling(s.x, s.y, s.z);
        //ScalingM = XMMatrixTranspose(ScalingM);

        aiQuaternion q;
        CalcInterpolatedRotation(q, AnimationTime, pNodeAnim);
        //XMFLOAT4 prt;

        //prt.x = -q.x;
        //prt.y = -q.y;
        //prt.z = q.z;
        ////prt.w = q.w;
        //float tt = 1.0f - (prt.x * prt.x)
        //    - (prt.y * prt.y)
        //    - (prt.z * prt.z);
        //if (tt < 0.0f)
        //{
        //    prt.w = 0.0f;
        //}
        //else
        //{
        //    prt.w = -sqrtf(tt);
        //}
       
        //XMMATRIX RotationM =  XMMatrixRotationQuaternion(XMVectorSet(q.x, q.y, -q.z, q.w));
        //XMMatrixRotationX(XMConvertToRadians(90));

        XMMATRIX RotationM = XMMatrixRotationQuaternion(XMVectorSet(q.x, q.y, q.z, q.w)); 
        
        
        //RotationM *= XMMatrixRotationX(XM_PI/2);

        RotationM = XMMatrixTranspose(RotationM);
        //RotationM = RotationM* XMMatrixRotationZ(XM_PI);
        //RotationM *= XMMatrixRotationY(XMConvertToDegrees(180));


        aiVector3D t;
        CalcInterpolatedPosition(t, AnimationTime, pNodeAnim);
        XMMATRIX TranslationM = XMMatrixTranslation(t.x, t.y, t.z);
        //TranslationM = XMMatrixTranspose(TranslationM);


        //NodeTransformation = ScalingM * RotationM * TranslationM;
        //NodeTransformation = TranslationM * RotationM * ScalingM;
        XMFLOAT4X4 mat;
        XMStoreFloat4x4(&mat,RotationM);
        
        //mat._13 *= s.z; mat._23 *= -1; mat._33 *= -1;

        mat._11 *= s.x; mat._21 *= s.x; mat._31 *= s.x;
        mat._12 *= s.y; mat._22 *= s.y; mat._32 *= s.y;
        mat._13 *= s.z; mat._23 *= s.z; mat._33 *= s.z;
        mat._14 = t.x; mat._24 = t.y; mat._34 = t.z;

        NodeTransformation = XMLoadFloat4x4(&mat);





        //XMFLOAT4X4 mat;
        //XMStoreFloat4x4(&mat, RotationM);

        //mat._11 *= s.x; mat._21 *= s.x; mat._31 *= s.x;
        //mat._12 *= s.y; mat._22 *= s.y; mat._32 *= s.y;
        //mat._13 *= s.z; mat._23 *= s.z; mat._33 *= s.z;
        //mat._14 = t.x; mat._24 = t.y; mat._34 = t.z;

        //NodeTransformation = XMLoadFloat4x4(&mat);



        //// transpose to get DirectX style matrix
        //mat.Transpose();
        //NodeTransformation = RotationM * TranslationM*ScalingM ;
        NodeTransformation = XMMatrixTranspose(NodeTransformation);
        //I applied transpos
    }


    //XMMATRIX GlobalTransformation = ParentTransform * NodeTransformation;
    XMMATRIX GlobalTransformation = NodeTransformation ;
    GlobalTransformation *= ParentTransform;
    //XMMATRIX GlobalTransformation = NodeTransformation * ParentTransform;


    if (boneMapping.find(NodeName) != boneMapping.end()) {
        int BoneIndex = boneMapping[NodeName];

        //m_BoneInfo[BoneIndex].FinalTransformation = m_BoneInfo[BoneIndex].BoneOffset* m_GlobalInverseTransform *GlobalTransformation;
        m_BoneInfo[BoneIndex].FinalTransformation = m_BoneInfo[BoneIndex].BoneOffset*  GlobalTransformation* XMMatrixRotationZ(XM_PI);
    }

    for (UINT i = 0; i < pNode->mNumChildren; ++i) {

        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

//void Decompose(XMVECTOR* _vPosition, XMVECTOR* _qRotation, XMVECTOR* _vScale, bool _bFlip)
//{
//    // compute matrix determinant
//    XMMATRIX fDet;
//    XMMatrixDeterminant(fDet);
//    _bFlip = (fDet < 0.0f);
//
//    // extract
//    ExtractTranslation(_vPosition);
//    ExtractScale(_vScale);
//    if (fDet < 0.0f)
//    {
//        imSMatrix4D matIDNeg, matResult;
//        matIDNeg.SetIdentity();
//        matIDNeg.m_11 = matIDNeg.m_22 = matIDNeg.m_33 = -1.0f;
//        Multiply(*this, matIDNeg, matResult);
//        _qRotation.FromRotationMatrix(matResult);
//    }
//    else
//        _qRotation.FromRotationMatrix(*this);
//}
//
//void Recompose(const imSVector3DTemplate<T>& _vPosition,
//    const imSQuaternionTemplate<T>& _qRotation,
//    const imSVector3DTemplate<T>& _vScale, imBool _bFlip)
//{
//    imSMatrix4D oMatTemp0, oMatTemp1, oMatTemp2;
//    _oMatTemp0.SetScale(_vScale);
//    if (_bFlip)
//    {
//        _oMatTemp0.m_11 = -_oMatTemp0.m_11;
//        _oMatTemp0.m_22 = -_oMatTemp0.m_22;
//        _oMatTemp0.m_33 = -_oMatTemp0.m_33;
//    }
//
//    // then rotation
//    _qRotation.ToRotationMatrix(_oMatTemp1);
//    imSMatrix4D::Multiply(_oMatTemp0, _oMatTemp1, _oMatTemp2);
//
//    // finally position
//    _oMatTemp0.SetTranslation(_vPosition.x, _vPosition.y, _vPosition.z);
//    imSMatrix4D::Multiply(_oMatTemp2, _oMatTemp0, *this);
//}


void SkinnedMesh::BoneTransform(float TimeInSeconds, vector<XMMATRIX>& Transforms)
{
    XMMATRIX Identity= XMMatrixIdentity();
    float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);
    

    ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);

    Transforms.resize(bonesCount);

    for (unsigned int i = 0; i < bonesCount; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}


const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}