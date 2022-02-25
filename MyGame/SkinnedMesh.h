#pragma once
#include <map>
#include <vector>
#include <TextureMgr.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <DirectXMath.h>
#include "AssimpMesh.h"
#pragma pack_matrix( column_major )
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
    //std::vector<BoneInfo> boneMatrices;
    bool LoadMesh(ID3D11Device* device, ID3D11DeviceContext* md3dImmediateContext, TextureMgr* texMgr, std::string Filename, std::wstring directory);
    void processNode(aiNode* node, const aiScene* scene);
    AssimpMesh processMesh(aiMesh* mesh, const aiScene* scene);
    void Render(ID3D11DeviceContext* md3dImmediateContext, ID3DX11EffectTechnique* tech, bool tex);
    void LoadBones(int MeshIndex, const aiMesh* pMesh, std::vector<Vertex::PosNormalTexTanSkinned>& Bones);
    void AddBoneData(int BoneID, float Weight, Vertex::PosNormalTexTanSkinned* bones);
    unsigned int NumBones() const
    {
        return bonesCount;
    }

    void BoneTransform(float TimeInSeconds, vector<XMMATRIX>& Transforms);
    vector<AssimpMesh> m_Entries;
private:
#define NUM_BONES_PER_VEREX 4

    

    //struct VertexBoneData
    //{
    //    unsigned int IDs[NUM_BONES_PER_VEREX];
    //    float Weights[NUM_BONES_PER_VEREX];

    //    VertexBoneData()
    //    {
    //        Reset();
    //    };

    //    void Reset()
    //    {
    //       /* ZERO_MEM(IDs);
    //        ZERO_MEM(Weights);*/
    //    }

    //    void AddBoneData(unsigned int BoneID, float Weight);
    //};

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const XMMATRIX ParentTransform);
    //bool InitFromScene(const aiScene* pScene, const string& Filename);
    //void InitMesh(unsigned int MeshIndex,
    //    const aiMesh* paiMesh,
    //    vector<XMFLOAT3>& Positions,
    //    vector<XMFLOAT3>& Normals,
    //    vector<XMFLOAT2>& TexCoords,
    //    vector<VertexBoneData>& Bones,
    //    vector<unsigned int>& Indices);
    //void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
    //bool InitMaterials(const aiScene* pScene, const string& Filename);
    void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

    //enum VB_TYPES {
    //    INDEX_BUFFER,
    //    POS_VB,
    //    NORMAL_VB,
    //    TEXCOORD_VB,
    //    BONE_VB,
    //    NUM_VBs
    //};

 /*   unsigned int m_VAO;
    unsigned int m_Buffers[NUM_VBs];*/

    //struct MeshEntry {
    //    MeshEntry()
    //    {
    //        NumIndices = 0;
    //        BaseVertex = 0;
    //        BaseIndex = 0;
    //        MaterialIndex = INVALID_MATERIAL;
    //    }

    //    unsigned int NumIndices;
    //    unsigned int BaseVertex;
    //    unsigned int BaseIndex;
    //    unsigned int MaterialIndex;
    //};


    vector<ID3D11ShaderResourceView*> m_Textures;

    //map<string, unsigned int> m_BoneMapping; // maps a bone name to its index
    //unsigned int m_NumBones;
    vector<BoneInfo> m_BoneInfo;
    XMMATRIX m_GlobalInverseTransform;

    const aiScene* m_pScene;
    Assimp::Importer m_Importer;
};

