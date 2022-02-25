//***************************************************************************************
// SkinnedMeshDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates skinned mesh character animation.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//      Press '1' for wireframe
//
//***************************************************************************************
#include "PxPhysicsAPI.h"
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"
#include "Sky.h"
#include "RenderStates.h"
#include "ShadowMap.h"
#include "Ssao.h"
#include "TextureMgr.h"
#include "BasicModel.h"
#include "SkinnedModel.h"
#include "SkinnedMesh.h"
#include "AssimpModel.h"
#include "Terrain.h"
#include "ParticleSystem.h"
#include "FontSheet.h"
#include "SpriteBatch.h"
#include "Physics.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui/imgui.h"
#include "imgui/imguizmo/ImGuizmo.h"
#include "imgui/imgui/imgui_internal.h"
#include "Scene.h"

struct BoundingSphere
{
	BoundingSphere() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	XMFLOAT3 Center;
	float Radius;
};

//bool useWindow;
//float camDistance = 8.f;
//static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
//
//static const float identityMatrix[16] =
//{ 1.f, 0.f, 0.f, 0.f,
//	0.f, 1.f, 0.f, 0.f,
//	0.f, 0.f, 1.f, 0.f,
//	0.f, 0.f, 0.f, 1.f };
//
//void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
//{
//	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
//	static bool useSnap = false;
//	static float snap[3] = { 1.f, 1.f, 1.f };
//	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
//	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
//	static bool boundSizing = false;
//	static bool boundSizingSnap = false;
//
//	if (editTransformDecomposition)
//	{
//		if (ImGui::IsKeyPressed(90))
//			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//		if (ImGui::IsKeyPressed(69))
//			mCurrentGizmoOperation = ImGuizmo::ROTATE;
//		if (ImGui::IsKeyPressed(82)) // r Key
//			mCurrentGizmoOperation = ImGuizmo::SCALE;
//		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
//			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
//			mCurrentGizmoOperation = ImGuizmo::ROTATE;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
//			mCurrentGizmoOperation = ImGuizmo::SCALE;
//		if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
//			mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;
//		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
//		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
//		ImGui::InputFloat3("Tr", matrixTranslation);
//		ImGui::InputFloat3("Rt", matrixRotation);
//		ImGui::InputFloat3("Sc", matrixScale);
//		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);
//
//		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
//		{
//			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
//				mCurrentGizmoMode = ImGuizmo::LOCAL;
//			ImGui::SameLine();
//			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
//				mCurrentGizmoMode = ImGuizmo::WORLD;
//		}
//		if (ImGui::IsKeyPressed(83))
//			useSnap = !useSnap;
//		//ImGui::Checkbox("", &useSnap);
//		ImGui::SameLine();
//
//		switch (mCurrentGizmoOperation)
//		{
//		case ImGuizmo::TRANSLATE:
//			ImGui::InputFloat3("Snap", &snap[0]);
//			break;
//		case ImGuizmo::ROTATE:
//			ImGui::InputFloat("Angle Snap", &snap[0]);
//			break;
//		case ImGuizmo::SCALE:
//			ImGui::InputFloat("Scale Snap", &snap[0]);
//			break;
//		}
//		ImGui::Checkbox("Bound Sizing", &boundSizing);
//		if (boundSizing)
//		{
//			ImGui::PushID(3);
//			ImGui::Checkbox("", &boundSizingSnap);
//			ImGui::SameLine();
//			ImGui::InputFloat3("Snap", boundsSnap);
//			ImGui::PopID();
//		}
//	}
//
//	ImGuiIO& io = ImGui::GetIO();
//	float viewManipulateRight = io.DisplaySize.x;
//	float viewManipulateTop = 0;
//	static ImGuiWindowFlags gizmoWindowFlags = 0;
//	if (useWindow)
//	{
//		ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
//		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
//		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
//		ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
//		ImGuizmo::SetDrawlist();
//		float windowWidth = (float)ImGui::GetWindowWidth();
//		float windowHeight = (float)ImGui::GetWindowHeight();
//		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
//		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
//		viewManipulateTop = ImGui::GetWindowPos().y;
//		ImGuiWindow* window = ImGui::GetCurrentWindow();
//		gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
//	}
//	else
//	{
//		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
//	}
//
//	ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
//	//ImGuizmo::DrawCubes(cameraView, cameraProjection, matrix, 1);//gizmoCount
//	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
//
//	ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);
//
//	if (useWindow)
//	{
//		ImGui::End();
//		ImGui::PopStyleColor(1);
//	}
//}
 
class SkinnedMeshApp : public D3DApp 
{
public:
	SkinnedMeshApp(HINSTANCE hInstance);
	~SkinnedMeshApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene(); 

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void DrawSceneToSsaoNormalDepthMap();
	void DrawSceneToShadowMap();
	void DrawScreenQuad(ID3D11ShaderResourceView* srv);
	void BuildShadowTransform();
	void BuildShapeGeometryBuffers();
	void BuildSkullGeometryBuffers();
	void BuildScreenQuadGeometryBuffers();

private:
	FontSheet mFont;
	SpriteBatch mSpriteBatch;
	ID3D11ShaderResourceView* mImageSRV;
	TextureMgr mTexMgr;

	Sky* mSky;

	AssimpModel* mAssimpModel;
	Terrain mTerrain;
	ParticleSystem mFire;
	ID3D11ShaderResourceView* mRandomTexSRV;
	ID3D11ShaderResourceView* mFlareTexSRV;
	
	//AnimatedModel* mAssimpSkelModel;
	SkinnedMesh* mSkelAssimpModel;
	vector<XMFLOAT4X4> transs;
	vector<XMFLOAT4X4> transskel;
	SkinnedModel* mCharacterModel;
	SkinnedModel* mCharacterModelAss;
	SkinnedModelInstance mCharacterInstanceAss;
	SkinnedModelInstance mCharacterInstance1;
	SkinnedModelInstance mCharacterInstance2;

	ID3D11Buffer* mShapesVB;
	ID3D11Buffer* mShapesIB;

	ID3D11Buffer* mSkullVB;
	ID3D11Buffer* mSkullIB;

	ID3D11Buffer* mSkySphereVB;
	ID3D11Buffer* mSkySphereIB;

	ID3D11Buffer* mScreenQuadVB;
	ID3D11Buffer* mScreenQuadIB;

	ID3D11ShaderResourceView* mStoneTexSRV;
	ID3D11ShaderResourceView* mAssTex;
	ID3D11ShaderResourceView* mBrickTexSRV;

	ID3D11ShaderResourceView* mStoneNormalTexSRV;
	ID3D11ShaderResourceView* mBrickNormalTexSRV;

	BoundingSphere mSceneBounds;

	static const int SMapSize = 2048;
	ShadowMap* mSmap;
	XMFLOAT4X4 mLightView;
	XMFLOAT4X4 mLightProj;
	XMFLOAT4X4 mShadowTransform;

	Ssao* mSsao;

	float mLightRotationAngle;
	XMFLOAT3 mOriginalLightDir[3];
	DirectionalLight mDirLights[3];
	Material mGridMat;
	Material mBoxMat;
	Material mCylinderMat;
	Material mSphereMat;
	Material mSkullMat;

	// Define transformations from local spaces to world space.
	XMFLOAT4X4 mSphereWorld[10];
	XMFLOAT4X4 mCylWorld[10];
	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mAssWorld;
	XMFLOAT4X4 mAssSkelWorld;
	XMFLOAT4X4 mGridWorld;
	XMFLOAT4X4 mSkullWorld;
	XMFLOAT4X4 mBoxWorldguizmo;
 
	int mBoxVertexOffset;
	int mGridVertexOffset;
	int mSphereVertexOffset;
	int mCylinderVertexOffset;

	UINT mBoxIndexOffset;
	UINT mGridIndexOffset;
	UINT mSphereIndexOffset;
	UINT mCylinderIndexOffset;

	UINT mBoxIndexCount;
	UINT mGridIndexCount;
	UINT mSphereIndexCount;
	UINT mCylinderIndexCount;

	UINT mSkullIndexCount;
	bool mWalkCamMode;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool show_demo_window = true;
	bool show_another_window = true;

	float mTheta;
	float mPhi;
	float mRadius;
	int gizmoCount = 1;
	Scene scn;

	std::shared_ptr<Physics> mPhx; 
	bool redytoshoot;
	std::vector<PxRigidDynamic*> bulletVector;
 
	Camera mCam;

	POINT mLastMousePos;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	SkinnedMeshApp theApp(hInstance);
	
	if( !theApp.Init() )
		return 0;
	
	return theApp.Run();
}
 

SkinnedMeshApp::SkinnedMeshApp(HINSTANCE hInstance)
: D3DApp(hInstance), mSky(0), mCharacterModel(0),  
  mShapesVB(0), mShapesIB(0), mSkullVB(0), mRandomTexSRV(0), mFlareTexSRV(0), mSkullIB(0), mScreenQuadVB(0), mScreenQuadIB(0),
  mStoneTexSRV(0), mBrickTexSRV(0),
  mStoneNormalTexSRV(0), mBrickNormalTexSRV(0), 
  mSkullIndexCount(0), mSmap(0), mSsao(0), mWalkCamMode(false),
  mLightRotationAngle(0.0f)
{
	mMainWndCaption = L"Skinned Mesh Demo";
	
	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	mCam.SetPosition(0.0f, 2.0f, -15.0f);

	// Estimate the scene bounding sphere manually since we know how the scene was constructed.
	// The grid is the "widest object" with a width of 20 and depth of 30.0f, and centered at
	// the world space origin.  In general, you need to loop over every world space vertex
	// position and compute the bounding sphere.
	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 00.0f);
	mSceneBounds.Radius = sqrtf(10.0f*10.0f + 100.0f*100.0f);

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mGridWorld, I);

	XMMATRIX boxScale = XMMatrixScaling(3.0f, 1.0f, 3.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX skullScale = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	XMMATRIX skullOffset = XMMatrixTranslation(0.0f, 1.0f, 0.0f);
	XMStoreFloat4x4(&mSkullWorld, XMMatrixMultiply(skullScale, skullOffset));

	for(int i = 0; i < 5; ++i)
	{
		XMStoreFloat4x4(&mCylWorld[i*2+0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mCylWorld[i*2+1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f));

		XMStoreFloat4x4(&mSphereWorld[i*2+0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f));
		XMStoreFloat4x4(&mSphereWorld[i*2+1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f));
	}

	mDirLights[0].Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(1.0f, 0.9f, 0.9f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	// Shadow acne gets worse as we increase the slope of the polygon (from the
	// perspective of the light).
	//mDirLights[0].Direction = XMFLOAT3(5.0f/sqrtf(50.0f), -5.0f/sqrtf(50.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(125.0f), -5.0f/sqrtf(125.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(116.0f), -4.0f/sqrtf(116.0f), 0.0f);
	//mDirLights[0].Direction = XMFLOAT3(10.0f/sqrtf(109.0f), -3.0f/sqrtf(109.0f), 0.0f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, 0.0, -1.0f);

	mOriginalLightDir[0] = mDirLights[0].Direction;
	mOriginalLightDir[1] = mDirLights[1].Direction;
	mOriginalLightDir[2] = mDirLights[2].Direction;

	mGridMat.Ambient  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mGridMat.Diffuse  = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	mGridMat.Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 16.0f);
	mGridMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mCylinderMat.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mCylinderMat.Diffuse  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mCylinderMat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f);
	mCylinderMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mSphereMat.Ambient  = XMFLOAT4(0.3f, 0.4f, 0.5f, 1.0f);
	mSphereMat.Diffuse  = XMFLOAT4(0.2f, 0.3f, 0.4f, 1.0f);
	mSphereMat.Specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
	mSphereMat.Reflect  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	mBoxMat.Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mBoxMat.Diffuse  = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mBoxMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	mBoxMat.Reflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	mSkullMat.Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mSkullMat.Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mSkullMat.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 16.0f);
	mSkullMat.Reflect  = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
}

SkinnedMeshApp::~SkinnedMeshApp()
{
	SafeDelete(mCharacterModel);
	ReleaseCOM(mRandomTexSRV);
	ReleaseCOM(mFlareTexSRV);
	SafeDelete(mSky);
	SafeDelete(mSmap);
	SafeDelete(mSsao);

	ReleaseCOM(mShapesVB);
	ReleaseCOM(mShapesIB);
	ReleaseCOM(mSkullVB);
	ReleaseCOM(mSkullIB);
	ReleaseCOM(mScreenQuadVB);
	ReleaseCOM(mScreenQuadIB);

	Effects::DestroyAll();
	InputLayouts::DestroyAll(); 
	RenderStates::DestroyAll();
	mPhx.get()->cleanupPhysics();
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool SkinnedMeshApp::Init()
{
	if(!D3DApp::Init())
		return false;

	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);
	RenderStates::InitAll(md3dDevice);
	mPhx=make_shared<Physics>();
	mPhx.get()->initPhysics();
	mTexMgr.Init(md3dDevice);

	mSky  = new Sky(md3dDevice, L"Textures/desertcube1024.dds", 5000.0f);
	mSmap = new ShadowMap(md3dDevice, SMapSize, SMapSize);

	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	scn.Init(md3dDevice,mPhx);
	mSsao = new Ssao(md3dDevice, md3dImmediateContext, mClientWidth, mClientHeight, mCam.GetFovY(), mCam.GetFarZ());

	mStoneTexSRV = mTexMgr.CreateTexture(L"Textures/floor.dds");
	mBrickTexSRV = mTexMgr.CreateTexture(L"Textures/bricks.dds");
	mStoneNormalTexSRV = mTexMgr.CreateTexture(L"Textures/floor_nmap.dds");
	mBrickNormalTexSRV = mTexMgr.CreateTexture(L"Textures/bricks_nmap.dds");
	mAssTex = mTexMgr.CreateWICTexture(L"Textures/diffuse.png");

	BuildShapeGeometryBuffers();
	BuildSkullGeometryBuffers();
	BuildScreenQuadGeometryBuffers();

	mCharacterModelAss = new SkinnedModel();

	mCharacterModel = new SkinnedModel(md3dDevice, mTexMgr, "Models\\soldier.m3d", L"Textures\\");
	mCharacterInstance1.Model = mCharacterModel;
	mCharacterInstance2.Model = mCharacterModel;
	mCharacterInstance1.TimePos = 0.0f;
	mCharacterInstance2.TimePos = 0.0f;
	mCharacterInstance1.ClipName = "Take1";
	mCharacterInstance2.ClipName = "Take1";
	mCharacterInstance1.FinalTransforms.resize(mCharacterModel->SkinnedData.BoneCount());
	mCharacterInstance2.FinalTransforms.resize(mCharacterModel->SkinnedData.BoneCount());

	mAssimpModel = new AssimpModel();
	mAssimpModel->Load(md3dDevice, md3dImmediateContext, &mTexMgr, "D:\\Downloads\\Wolf Rigged and Game Ready\\Wolf_dae.dae", L"D:\\Downloads\\Wolf Rigged and Game Ready\\textures\\");

	mSkelAssimpModel = new SkinnedMesh();
	//AnimatedModelLoader* mAssimpSkelModelLoader;
	//mAssimpSkelModelLoader = new AnimatedModelLoader();
	AssimpMesh ret;
	//mSkelAssimpModel->LoadMesh(md3dDevice, md3dImmediateContext, &mTexMgr, "D:\\Downloads\\ogldev-source\\Content\\boblampclean.md5mesh", L"D:\\Downloads\\Wolf Rigged and Game Ready\\textures\\");
	//mSkelAssimpModel->LoadMesh(md3dDevice, md3dImmediateContext, &mTexMgr, "D:\\Downloads\\fbx\\Handgun_fbx_6.1_ASCII.fbx", L"D:\\Downloads\\fbx\\textures\\");
	//mSkelAssimpModel->LoadMesh(md3dDevice, md3dImmediateContext, &mTexMgr, "D:\\Downloads\\Wolf Rigged and Game Ready\\Wolf_dae.dae", L"D:\\Downloads\\Wolf Rigged and Game Ready\\textures\\");
	mSkelAssimpModel->LoadMesh(md3dDevice, md3dImmediateContext, &mTexMgr, "C:\\mydev\\DirectX11_Tests\\DirectX11_Tests\\MyGame\\Models\\model.dae", L"D:\\Downloads\\Wolf Rigged and Game Ready\\textures\\");
	//ret= mAssimpSkelModelLoader->LoadMesh(md3dDevice, md3dImmediateContext, &mTexMgr, "C:\\mydev\\OpenGL-Animation\\Resources\\res\\model.dae", L"D:\\Downloads\\Wolf Rigged and Game Ready\\textures\\");
	//mAssimpSkelModel->Load(md3dDevice, md3dImmediateContext, &mTexMgr, "C:\\mydev\\ritchardsoft\\DX11\\SkinnedModels\\Models\\magician.X", L"C:\\mydev\\ritchardsoft\\DX11\\SkinnedModels\\Textures\\");
	// 
	//mAssimpSkelModel = new AnimatedModel(ret,nullptr,0);
	// 
	// Reflect to change coordinate system from the RHS the data was exported out as.
	//Joint join;
	//XMFLOAT4X4 globinv;
	//SkinnedData skinInfo;
	//AnimatedModelLoader aml;
	//ret=aml.LoadMesh(md3dDevice, md3dImmediateContext, &mTexMgr, "C:\\mydev\\OpenGL-Animation\\Resources\\res\\model.dae", L"D:\\Downloads\\Wolf Rigged and Game Ready\\textures\\",&join, &globinv, &skinInfo);
	//Joint jo;
	//jo.localBindTransform = aml.m_BoneInfo[0].BoneOffset;
	//jo.index = 0;
	//for (auto i = aml.boneMapping.begin(); i !=aml.boneMapping.end(); i++)
	//{
	//	if (i->second == 0)
	//	{
	//		jo.name = i->first;
	//	}
	//}

	//animod.model = ret;
	//animod.rootJoint = join;
	//animod.jointCount = aml.bonesCount;

	//Animation anima = aml.LoadAnimation();
	//animod.currentAnimation = anima;
	//animod.globinv = globinv;

	//mCharacterModelAss = new SkinnedModel();
	//mCharacterModelAss->SkinnedData = skinInfo;
	//mCharacterModelAss->AssModelMesh = ret;
	//mCharacterInstanceAss.Model = mCharacterModelAss;
	//mCharacterInstanceAss.TimePos = 0.0f;
	//mCharacterInstanceAss.ClipName = "run";
	//mCharacterInstanceAss.FinalTransforms.resize(mCharacterModelAss->SkinnedData.BoneCount());

	XMMATRIX modelScale = XMMatrixScaling(0.05f, 0.05f, -0.05f);
	XMMATRIX modelRot   = XMMatrixRotationY(MathHelper::Pi);
	XMMATRIX modelOffset = XMMatrixTranslation(-2.0f, 0.0f, -7.0f);
	XMStoreFloat4x4(&mCharacterInstance1.World, modelScale*modelRot*modelOffset);

	modelOffset = XMMatrixTranslation(2.0f, 0.0f, -7.0f);
	XMStoreFloat4x4(&mCharacterInstance2.World, modelScale*modelRot*modelOffset);

	modelOffset = XMMatrixTranslation(-4.0f, 0.0f, -7.0f);
	XMStoreFloat4x4(&mCharacterInstanceAss.World, modelScale * modelRot * modelOffset);

	modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	modelRot = XMMatrixRotationZ(0);
	modelOffset = XMMatrixTranslation(5.0f, 15.0f, -8.0f);
	XMStoreFloat4x4(&mAssWorld, modelScale * modelRot * modelOffset);
	XMStoreFloat4x4(&mBoxWorldguizmo, modelScale * modelRot * modelOffset);

	modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	modelRot = XMMatrixRotationY(0.0f);
	modelOffset = XMMatrixTranslation(4.0f, 2.0f, -7.0f);
	XMStoreFloat4x4(&mAssSkelWorld, modelScale * modelRot * modelOffset);

	Terrain::InitInfo tii;
	tii.HeightMapFilename = L"Textures/terrain2.raw";
	tii.LayerMapFilename0 = L"Textures/grass.dds";
	tii.LayerMapFilename1 = L"Textures/darkdirt.dds";
	tii.LayerMapFilename2 = L"Textures/stone.dds";
	tii.LayerMapFilename3 = L"Textures/lightdirt.dds";
	tii.LayerMapFilename4 = L"Textures/snow.dds";
	tii.BlendMapFilename = L"Textures/blend.dds";
	tii.HeightScale = 50.0f;
	tii.HeightmapWidth = 2049;
	tii.HeightmapHeight = 2049;
	tii.CellSpacing = 0.5f;

	mTerrain.Init(md3dDevice, md3dImmediateContext, tii);
	
	mRandomTexSRV = d3dHelper::CreateRandomTexture1DSRV(md3dDevice);

	std::vector<std::wstring> flares;
	flares.push_back(L"Textures\\flare0.dds");
	d3dHelper::CreateTextureViewArrayFromFiles(md3dDevice, md3dImmediateContext, flares, &mFlareTexSRV);

	mFire.Init(md3dDevice, Effects::FireFX, mFlareTexSRV, mRandomTexSRV, 500);
	mFire.SetEmitPos(XMFLOAT3(4.0f, 16.0f, -2.0f));

	HR(mFont.Initialize(md3dDevice, L"Times New Roman", 96.0f, FontSheet::FontStyleBold, true));

	HR(mSpriteBatch.Initialize(md3dDevice));

	ID3D11Resource* texResource = nullptr;

	HR(DirectX::CreateDDSTextureFromFile(md3dDevice, L"Textures/WoodCrate01.dds", &texResource, &mImageSRV));

	ReleaseCOM(texResource); // view saves reference




	 // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(mhMainWnd);
	ImGui_ImplDX11_Init(md3dDevice, md3dImmediateContext);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);


	return true;
}

void SkinnedMeshApp::OnResize()
{
	D3DApp::OnResize();

	mCam.SetLens(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);

	if( mSsao )
	{
		mSsao->OnSize(mClientWidth, mClientHeight, mCam.GetFovY(), mCam.GetFarZ());
	}
}

void SkinnedMeshApp::UpdateScene(float dt)
{
	//
	// Control the camera.
	//
	if( GetAsyncKeyState('W') & 0x8000 )
		mCam.Walk(10.0f*dt);

	if( GetAsyncKeyState('S') & 0x8000 )
		mCam.Walk(-10.0f*dt);

	if( GetAsyncKeyState('A') & 0x8000 )
		mCam.Strafe(-10.0f*dt);

	if( GetAsyncKeyState('D') & 0x8000 )
		mCam.Strafe(10.0f*dt);

	//
// Walk/fly mode
//
	if (GetAsyncKeyState('2') & 0x8000)
		mWalkCamMode = true;
	if (GetAsyncKeyState('3') & 0x8000)
		mWalkCamMode = false;

	XMMATRIX modelScale = XMMatrixScaling(0.05f, 0.05f, -0.05f);
	XMMATRIX modelRot = XMMatrixRotationY(MathHelper::Pi);
	XMMATRIX modelOffset = XMMatrixTranslation(-2.0f, 0.0f, -7.0f);

	modelScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	modelRot = XMMatrixRotationZ(0);

	{
		mCam.Walk(20);

		XMFLOAT3 camPos = mCam.GetPosition();
		float y = mTerrain.GetHeight(camPos.x, camPos.z);
		modelOffset = XMMatrixTranslation(camPos.x, y, camPos.z);
		XMStoreFloat4x4(&mAssWorld, modelScale * modelRot * modelOffset);
		mCam.Walk(-20);
	}
	// 
	// Clamp camera to terrain surface in walk mode.
	//
	if (mWalkCamMode)
	{
		XMFLOAT3 camPos = mCam.GetPosition();
		float y = mTerrain.GetHeight(camPos.x, camPos.z);
		mCam.SetPosition(camPos.x, y + 2.0f, camPos.z);
	}

	//
	// Animate the character.
	// 

	if (GetAsyncKeyState('R') & 0x8000)
	{
		mFire.Reset();
	}

	mFire.Update(dt, mTimer.TotalTime());

	vector<XMMATRIX> Transforms;

	mSkelAssimpModel->BoneTransform(mTimer.TotalTime(), Transforms);
	transs.resize(Transforms.size());
	for (size_t i = 0; i < Transforms.size(); i++)
	{
		XMFLOAT4X4 trans;
		XMStoreFloat4x4(&trans, Transforms[i]);
		transs[i]=trans;
	}
	Transforms.clear();

	//animod.update(dt);
	//transskel.clear();
	//transskel=animod.getJointTransforms();
	/*Transforms =mAssimpSkelModel->Animator.GetTransforms(mTimer.TotalTime());

	transskel.clear();
	for (size_t i = 0; i < 40; i++)
	{
		XMFLOAT4X4 trans;
		XMStoreFloat4x4(&trans, Transforms[i]);
		transskel.push_back(trans);
	}*/
	
	mCharacterInstance1.Update(dt);
	mCharacterInstance2.Update(dt);
	//mCharacterInstanceAss.Update(dt);

	//
	// Animate the lights (and hence shadows).
	//

	BuildShadowTransform();

	mPhx.get()->stepPhysics();
	if (GetAsyncKeyState(VK_SPACE))
	{
		if (redytoshoot == true)
		{
			PxRigidDynamic* prd;
			prd = mPhx.get()->createDynamic(PxTransform(PxVec3(mCam.GetPosition().x, mCam.GetPosition().y, mCam.GetPosition().z)), PxSphereGeometry(1), PxVec3(mCam.GetLook().x, mCam.GetLook().y, mCam.GetLook().z) * 50);
			bulletVector.push_back(prd);
			redytoshoot = false;
		}
	}

	if (GetAsyncKeyState(VK_SPACE) == false)
	{
		redytoshoot = true;
	}

	mCam.UpdateViewMatrix();
}

void SkinnedMeshApp::DrawScene()
{
	//
	// Render the scene to the shadow map.
	//

	mSmap->BindDsvAndSetNullRenderTarget(md3dImmediateContext);

	DrawSceneToShadowMap();

	md3dImmediateContext->RSSetState(0);

	//
	// Render the view space normals and depths.  This render target has the
	// same dimensions as the back buffer, so we can use the screen viewport.
	// This render pass is needed to compute the ambient occlusion.
	// Notice that we use the main depth/stencil buffer in this pass.  
	//

	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	mSsao->SetNormalDepthRenderTarget(mDepthStencilView);


	
	DrawSceneToSsaoNormalDepthMap();

	//
	// Now compute the ambient occlusion.
	//

	mSsao->ComputeSsao(mCam);
	mSsao->BlurAmbientMap(2);

	//
	// Restore the back and depth buffer and viewport to the OM stage.
	//
	ID3D11RenderTargetView* renderTargets[1] = {mRenderTargetView};
	md3dImmediateContext->OMSetRenderTargets(1, renderTargets, mDepthStencilView);
	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));

	// We already laid down scene depth to the depth buffer in the Normal/Depth map pass,
	// so we can set the depth comparison test to “EQUALS.”  This prevents any overdraw
	// in this rendering pass, as only the nearest visible pixels will pass this depth
	// comparison test.

	md3dImmediateContext->OMSetDepthStencilState(RenderStates::EqualsDSS, 0);



	md3dImmediateContext->RSSetState(0);
	XMMATRIX view     = mCam.View();
	XMMATRIX proj     = mCam.Proj();
	XMMATRIX viewProj = mCam.ViewProj();



	// Set per frame constants.
	Effects::BasicFX->SetDirLights(mDirLights);
	Effects::BasicFX->SetEyePosW(mCam.GetPosition());
	Effects::BasicFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::BasicFX->SetShadowMap(mSmap->DepthMapSRV());
	Effects::BasicFX->SetSsaoMap(mSsao->AmbientSRV());

	Effects::NormalMapFX->SetDirLights(mDirLights);
	Effects::NormalMapFX->SetEyePosW(mCam.GetPosition());
	Effects::NormalMapFX->SetCubeMap(mSky->CubeMapSRV());
	Effects::NormalMapFX->SetShadowMap(mSmap->DepthMapSRV());
	Effects::NormalMapFX->SetSsaoMap(mSsao->AmbientSRV());

	// Figure out which technique to use for different geometry.
	ID3DX11EffectTechnique* activeTech        = Effects::NormalMapFX->Light3TexTech;
	ID3DX11EffectTechnique* activeSphereTech  = Effects::BasicFX->Light3ReflectTech;
	ID3DX11EffectTechnique* activeSkullTech   = Effects::BasicFX->Light3ReflectTech;
	ID3DX11EffectTechnique* activeSkinnedTech = Effects::NormalMapFX->Light3TexSkinnedTech;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX shadowTransform = XMLoadFloat4x4(&mShadowTransform);


	//
	// Draw the grid, cylinders, and box without any cubemap reflection.
	// 
	

     
	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);
	
    D3DX11_TECHNIQUE_DESC techDesc;

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	activeTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		auto rview = scn.registry.view<component::Shape, component::Transform>();
		for (auto entity : rview)
		{
			auto& shap = rview.get<component::Shape>(entity);
			auto& trans = rview.get<component::Transform>(entity);



			md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
			md3dImmediateContext->IASetVertexBuffers(0, 1, &shap.ShapeVB, &stride, &offset);
			md3dImmediateContext->IASetIndexBuffer(shap.ShapeIB, DXGI_FORMAT_R32_UINT, 0);
			// Draw the box.
			world = XMLoadFloat4x4(&trans.Transform);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
			Effects::NormalMapFX->SetMaterial(mBoxMat);
			Effects::NormalMapFX->SetDiffuseMap(mBrickTexSRV);
			Effects::NormalMapFX->SetNormalMap(mBrickNormalTexSRV);

			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(36, 0, 0);
		}
	}

	stride = sizeof(Vertex::PosNormalTexTan);
	offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

    activeTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		// Draw the grid.
		world = XMLoadFloat4x4(&mGridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::NormalMapFX->SetWorld(world);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
		Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));
		Effects::NormalMapFX->SetMaterial(mGridMat);
		Effects::NormalMapFX->SetDiffuseMap(mStoneTexSRV);
		Effects::NormalMapFX->SetNormalMap(mStoneNormalTexSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&mBoxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::NormalMapFX->SetWorld(world);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
		Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));
		Effects::NormalMapFX->SetMaterial(mBoxMat);
		Effects::NormalMapFX->SetDiffuseMap(mBrickTexSRV);
		Effects::NormalMapFX->SetNormalMap(mBrickNormalTexSRV);

		activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// Draw the cylinders.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mCylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::NormalMapFX->SetWorld(world);
			Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
			Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
			Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
			Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
			Effects::NormalMapFX->SetMaterial(mCylinderMat);
			Effects::NormalMapFX->SetDiffuseMap(mBrickTexSRV);
			Effects::NormalMapFX->SetNormalMap(mBrickNormalTexSRV);
 
			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		world = XMLoadFloat4x4(&mAssWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::NormalMapFX->SetWorld(world);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
		Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
		Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
		//Effects::NormalMapFX->SetMaterial(mModelInstances[2].Model->Mat[0]);
		//Effects::NormalMapFX->SetDiffuseMap(mModelInstances[2].Model->DiffuseMapSRV[0]);
		//Effects::NormalMapFX->SetNormalMap(mModelInstances[2].Model->NormalMapSRV[0]);

		for (UINT subset = 0; subset < mAssimpModel->meshes.size(); ++subset)
		{
			//Effects::NormalMapFX->SetMaterial(mAssimpModel->meshes[subset].Mat[0]);
			//Effects::NormalMapFX->SetDiffuseMap(mAssimpModel->meshes[subset].DiffuseMapSRV[0]);
			if (mAssimpModel->meshes[subset].NormalMapSRV.empty() == false)
			{
				Effects::NormalMapFX->SetNormalMap(mAssimpModel->meshes[subset].NormalMapSRV[0]);
			}
			activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			mAssimpModel->meshes[subset].Draw(md3dImmediateContext, activeTech, false);
		}

		// draw bullets
		for (size_t i = 0; i < bulletVector.size(); i++)
		{

				// Set per object constants.
				XMFLOAT4X4 tmp;
				const PxMat44 shapePose = bulletVector[i]->getGlobalPose();
				tmp._11 = shapePose.column0.x;
				tmp._12 = shapePose.column0.y;
				tmp._13 = shapePose.column0.z;
				tmp._14 = shapePose.column0.w;

				tmp._21 = shapePose.column1.x;
				tmp._22 = shapePose.column1.y;
				tmp._23 = shapePose.column1.z;
				tmp._24 = shapePose.column1.w;

				tmp._31 = shapePose.column2.x;
				tmp._32 = shapePose.column2.y;
				tmp._33 = shapePose.column2.z;
				tmp._34 = shapePose.column2.w;

				tmp._41 = shapePose.column3.x;
				tmp._42 = shapePose.column3.y;
				tmp._43 = shapePose.column3.z;
				tmp._44 = shapePose.column3.w;

				XMMATRIX world = XMLoadFloat4x4(&tmp);
				XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
				XMMATRIX worldViewProj = world * view * proj;

				Effects::NormalMapFX->SetWorld(world);
				Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
				Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
				Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj * toTexSpace);
				Effects::NormalMapFX->SetShadowTransform(world * shadowTransform);
				Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));
				Effects::NormalMapFX->SetMaterial(mCylinderMat);
				Effects::NormalMapFX->SetDiffuseMap(mBrickTexSRV);
				Effects::NormalMapFX->SetNormalMap(mBrickNormalTexSRV);

				activeTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
				md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}
    }

	//
	// Draw the spheres with cubemap reflection.
	//
 
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

	activeSphereTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		// Draw the spheres.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BasicFX->SetWorld(world);
			Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BasicFX->SetWorldViewProj(worldViewProj);
			Effects::BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
			Effects::BasicFX->SetShadowTransform(world*shadowTransform);
			Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
			Effects::BasicFX->SetMaterial(mSphereMat);
 
			activeSphereTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}
	}

	stride = sizeof(Vertex::Basic32);
    offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mSkullVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mSkullIB, DXGI_FORMAT_R32_UINT, 0);

	activeSkullTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		// Draw the skull.
		world = XMLoadFloat4x4(&mSkullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);
		Effects::BasicFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		Effects::BasicFX->SetMaterial(mSkullMat);
		Effects::BasicFX->SetShadowTransform(world*shadowTransform);
	 
		activeSkullTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);
	}

	world = XMLoadFloat4x4(&mTerrain.mWorld);
	Effects::TerrainFX->SetshadowTransform(world* shadowTransform);//meight not be right
	Effects::TerrainFX->SetSsaoMap(mSsao->AmbientSRV());
	Effects::TerrainFX->SetShadowMap(mSmap->DepthMapSRV());

	mTerrain.Draw(md3dImmediateContext, mCam, mDirLights, mSsao->AmbientSRV(), mSmap->DepthMapSRV(), shadowTransform);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	// Draw the animated characters.
	//

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	activeSkinnedTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		// Instance 1

		world = XMLoadFloat4x4(&mCharacterInstance1.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::NormalMapFX->SetWorld(world);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
		Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
		Effects::NormalMapFX->SetBoneTransforms(
			&mCharacterInstance1.FinalTransforms[0], 
			mCharacterInstance1.FinalTransforms.size());

		for(UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
		{
			Effects::NormalMapFX->SetMaterial(mCharacterInstance1.Model->Mat[subset]);
			Effects::NormalMapFX->SetDiffuseMap(mCharacterInstance1.Model->DiffuseMapSRV[subset]);
			Effects::NormalMapFX->SetNormalMap(mCharacterInstance1.Model->NormalMapSRV[subset]);

			activeSkinnedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			mCharacterInstance1.Model->ModelMesh.Draw(md3dImmediateContext, subset);
		}

		// Instance 2

		world = XMLoadFloat4x4(&mCharacterInstance2.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::NormalMapFX->SetWorld(world);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj*toTexSpace);
		Effects::NormalMapFX->SetShadowTransform(world*shadowTransform);
		Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));
		
		Effects::NormalMapFX->SetBoneTransforms(
			&mCharacterInstance2.FinalTransforms[0], 
			mCharacterInstance2.FinalTransforms.size());

		for(UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
		{
			Effects::NormalMapFX->SetMaterial(mCharacterInstance2.Model->Mat[subset]);
			Effects::NormalMapFX->SetDiffuseMap(mCharacterInstance2.Model->DiffuseMapSRV[subset]);
			Effects::NormalMapFX->SetNormalMap(mCharacterInstance2.Model->NormalMapSRV[subset]);

			activeSkinnedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			mCharacterInstance2.Model->ModelMesh.Draw(md3dImmediateContext, subset);
		}

		//world = XMLoadFloat4x4(&mCharacterInstanceAss.World);
		//worldInvTranspose = MathHelper::InverseTranspose(world);
		//worldViewProj = world * view * proj;

		//Effects::NormalMapFX->SetWorld(world);
		//Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		//Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		//Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj* toTexSpace);
		//Effects::NormalMapFX->SetShadowTransform(world* shadowTransform);
		//Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		//Effects::NormalMapFX->SetBoneTransforms(
		//	&mCharacterInstanceAss.FinalTransforms[0],
		//	mCharacterInstanceAss.FinalTransforms.size());

		//for (UINT subset = 0; subset < 1; ++subset)
		//{
		//	//Effects::NormalMapFX->SetMaterial(mCharacterInstanceAss.Model->Mat[subset]);
		//	//Effects::NormalMapFX->SetDiffuseMap(mCharacterInstanceAss.Model->DiffuseMapSRV[subset]);
		//	//Effects::NormalMapFX->SetNormalMap(mCharacterInstanceAss.Model->NormalMapSRV[subset]);

		//	activeSkinnedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		//	mCharacterInstanceAss.Model->AssModelMesh.Draw(md3dImmediateContext, activeSkinnedTech, false);
		//}

		world = XMLoadFloat4x4(&mAssWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::NormalMapFX->SetWorld(world);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj* toTexSpace);
		Effects::NormalMapFX->SetShadowTransform(world* shadowTransform);
		Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		

		Effects::NormalMapFX->SetBoneTransforms(
			&transs[0],
			transs.size());
		//Effects::NormalMapFX->SetMaterial(mModelInstances[2].Model->Mat[0]);
		//Effects::NormalMapFX->SetDiffuseMap(mModelInstances[2].Model->DiffuseMapSRV[0]);
		//Effects::NormalMapFX->SetNormalMap(mModelInstances[2].Model->NormalMapSRV[0]);

		for (UINT subset = 0; subset < mSkelAssimpModel->m_Entries.size(); ++subset)
		{
			Effects::NormalMapFX->SetMaterial(mCharacterInstance2.Model->Mat[0]);
			Effects::NormalMapFX->SetDiffuseMap(mAssTex);
			if (mSkelAssimpModel->m_Entries[subset].NormalMapSRV.empty() == false)
			{
				Effects::NormalMapFX->SetNormalMap(mSkelAssimpModel->m_Entries[subset].NormalMapSRV[0]);
			}
			activeSkinnedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			mSkelAssimpModel->m_Entries[subset].Draw(md3dImmediateContext, activeSkinnedTech, false);
		}

		//world = XMLoadFloat4x4(&mAssSkelWorld);
		//worldInvTranspose = MathHelper::InverseTranspose(world);
		//worldViewProj = world * view * proj;

		//Effects::NormalMapFX->SetWorld(world);
		//Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		//Effects::NormalMapFX->SetWorldViewProj(worldViewProj);
		//Effects::NormalMapFX->SetWorldViewProjTex(worldViewProj* toTexSpace);
		//Effects::NormalMapFX->SetShadowTransform(world* shadowTransform);
		//Effects::NormalMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));



		//Effects::NormalMapFX->SetBoneTransforms(
		//	&transskel[0],
		//	transskel.size());
		////Effects::NormalMapFX->SetMaterial(mModelInstances[2].Model->Mat[0]);
		////Effects::NormalMapFX->SetDiffuseMap(mModelInstances[2].Model->DiffuseMapSRV[0]);
		////Effects::NormalMapFX->SetNormalMap(mModelInstances[2].Model->NormalMapSRV[0]);

		//for (UINT subset = 0; subset < 1; ++subset)
		//{
		//	//Effects::NormalMapFX->SetMaterial(mAssimpModel->meshes[subset].Mat[0]);
		//	//Effects::NormalMapFX->SetDiffuseMap(mAssimpModel->meshes[subset].DiffuseMapSRV[0]);
		//	if (animod.getModel().NormalMapSRV.empty() == false)
		//	{
		//		Effects::NormalMapFX->SetNormalMap(animod.getModel().NormalMapSRV[0]);
		//	}
		//	activeSkinnedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//	animod.getModel().Draw(md3dImmediateContext, activeSkinnedTech, false);
		//}
	}
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };


	// Turn off wireframe.


	//mTerrain.Draw(md3dImmediateContext, mCam, mDirLights);

	// Restore from RenderStates::EqualsDSS
	md3dImmediateContext->OMSetDepthStencilState(0, 0);





	// Debug view SSAO map.
	//DrawScreenQuad(mSsao->AmbientSRV());
	md3dImmediateContext->RSSetState(0);
	mSky->Draw(md3dImmediateContext, mCam);

	//uncomment if you want some ui transperency
	//blendFactor[0] = { 1.0f };
	//blendFactor[1] = { 1.0f };
	//blendFactor[2] = { 1.0f };
	//blendFactor[3] = { 1.0f };

	//md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	mFire.SetEyePos(mCam.GetPosition());
	mFire.Draw(md3dImmediateContext, mCam);

	//sprite
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	mSpriteBatch.BeginBatch(mImageSRV);

	// Draw the sprite in each corner of the screen with a different tint.
	CD3D11_RECT r1(0, 0, 200, 200);
	CD3D11_RECT r2(mClientWidth - 200, 0, mClientWidth, 200);
	CD3D11_RECT r3(0, mClientHeight - 200, 200, mClientHeight);
	CD3D11_RECT r4(mClientWidth - 200, mClientHeight - 200, mClientWidth, mClientHeight);

	mSpriteBatch.Draw(r1, DirectX::PackedVector::XMCOLOR::XMCOLOR(0xffffffff));
	mSpriteBatch.Draw(r2, DirectX::PackedVector::XMCOLOR::XMCOLOR(0xffff0000));
	mSpriteBatch.Draw(r3, DirectX::PackedVector::XMCOLOR::XMCOLOR(0xff00ff00));
	mSpriteBatch.Draw(r4, DirectX::PackedVector::XMCOLOR::XMCOLOR(0xff00ffff));

	mSpriteBatch.EndBatch(md3dImmediateContext);


	md3dImmediateContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

	std::wstring text = L"Hallo";

	// Calculate the text width.
	int textWidth = 0;
	for (UINT i = 0; i < text.size(); ++i)
	{
		WCHAR character = text[i];
		if (character == ' ')
		{
			textWidth += mFont.GetSpaceWidth();
		}
		else
		{
			const CD3D11_RECT& r = mFont.GetCharRect(text[i]);
			textWidth += (r.right - r.left + 1);
		}
	}

	// Center the text in the screen.
	POINT textPos;
	textPos.x = (mClientWidth - textWidth) / 2;
	textPos.y = (mClientHeight - mFont.GetCharHeight()) / 2;

	mSpriteBatch.DrawString(md3dImmediateContext, mFont, text, textPos, DirectX::PackedVector::XMCOLOR::XMCOLOR(0xffffff00));

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}


	//// 3. Show another simple window.
	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		show_another_window = false;
	//	ImGui::End();
	//}

	scn.ShowEntityList(mCam);

	// Rendering
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	// restore default
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	//sprite end

	// restore default states, as the SkyFX changes them in the effect file.
	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);

	// Unbind shadow map and AmbientMap as a shader input because we are going to render
	// to it next frame.  These textures can be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	md3dImmediateContext->PSSetShaderResources(0, 16, nullSRV);

	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->OMSetDepthStencilState(0, 0);
	md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff);



	HR(mSwapChain->Present(0, 0));
}

void SkinnedMeshApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void SkinnedMeshApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void SkinnedMeshApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		mCam.Pitch(dy);
		mCam.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void SkinnedMeshApp::DrawSceneToSsaoNormalDepthMap()
{
	XMMATRIX view     = mCam.View();
	XMMATRIX proj     = mCam.Proj();
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	ID3DX11EffectTechnique* tech = Effects::SsaoNormalDepthFX->NormalDepthTech;
	ID3DX11EffectTechnique* animatedTech = Effects::SsaoNormalDepthFX->NormalDepthSkinnedTech;

	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldView;
	XMMATRIX worldInvTransposeView;
	XMMATRIX worldViewProj;

	//
	// Draw the grid, cylinders, spheres and box.
	//mTerrain.DrawSSAO(md3dImmediateContext, mCam, mDirLights);




	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);
     
    D3DX11_TECHNIQUE_DESC techDesc;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	tech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		auto rview = scn.registry.view<component::Shape, component::Transform>();
		for (auto entity : rview)
		{
			auto& shap = rview.get<component::Shape>(entity);
			auto& trans = rview.get<component::Transform>(entity);

			md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
			md3dImmediateContext->IASetVertexBuffers(0, 1, &shap.ShapeVB, &stride, &offset);
			md3dImmediateContext->IASetIndexBuffer(shap.ShapeIB, DXGI_FORMAT_R32_UINT, 0);
			// Draw the box.
			world = XMLoadFloat4x4(&trans.Transform);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView = world * view;
			worldInvTransposeView = worldInvTranspose * view;
			worldViewProj = world * view * proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(36, 0, 0);
		}
	}

	stride = sizeof(Vertex::PosNormalTexTan);
	offset = 0;
	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

    tech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		// Draw the grid.
		world = XMLoadFloat4x4(&mGridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView     = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&mBoxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView     = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));

		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// Draw the cylinders.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mCylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView     = world*view;
			worldInvTransposeView = worldInvTranspose*view;
			worldViewProj = world*view*proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// draw bullets
		for (size_t i = 0; i < bulletVector.size(); i++)
		{

			// Set per object constants.
			XMFLOAT4X4 tmp;
			const PxMat44 shapePose = bulletVector[i]->getGlobalPose();
			tmp._11 = shapePose.column0.x;
			tmp._12 = shapePose.column0.y;
			tmp._13 = shapePose.column0.z;
			tmp._14 = shapePose.column0.w;

			tmp._21 = shapePose.column1.x;
			tmp._22 = shapePose.column1.y;
			tmp._23 = shapePose.column1.z;
			tmp._24 = shapePose.column1.w;

			tmp._31 = shapePose.column2.x;
			tmp._32 = shapePose.column2.y;
			tmp._33 = shapePose.column2.z;
			tmp._34 = shapePose.column2.w;

			tmp._41 = shapePose.column3.x;
			tmp._42 = shapePose.column3.y;
			tmp._43 = shapePose.column3.z;
			tmp._44 = shapePose.column3.w;

			world = XMLoadFloat4x4(&tmp);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView = world * view;
			worldInvTransposeView = worldInvTranspose * view;
			worldViewProj = world * view * proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));

			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// Draw the spheres.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldView     = world*view;
			worldInvTransposeView = worldInvTranspose*view;
			worldViewProj = world*view*proj;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());

			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}

		world = XMLoadFloat4x4(&mAssWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		worldView = world * view;
		worldInvTransposeView = worldInvTranspose * view;

		Effects::SsaoNormalDepthFX->SetWorldView(world);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		for (UINT subset = 0; subset < mAssimpModel->meshes.size(); ++subset)
		{
			//Effects::NormalMapFX->SetMaterial(mAssimpModel->Mat[subset]);
			//Effects::SsaoNormalDepthFX->SetDiffuseMap(mAssimpModel->meshes[subset].DiffuseMapSRV[0]);
			//Effects::NormalMapFX->SetNormalMap(mAssimpModel->NormalMapSRV[subset]);
			tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			mAssimpModel->meshes[subset].Draw(md3dImmediateContext, tech, false);
		}
    }
	//
	// Draw the skull.
	//

	stride = sizeof(Vertex::Basic32);
    offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mSkullVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mSkullIB, DXGI_FORMAT_R32_UINT, 0);

	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		world = XMLoadFloat4x4(&mSkullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView     = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());

		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);
	}


	//
	// Draw the animated characters.
	//

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	animatedTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		// Instance 1

		world = XMLoadFloat4x4(&mCharacterInstance1.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView     = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
		Effects::SsaoNormalDepthFX->SetBoneTransforms(
			&mCharacterInstance1.FinalTransforms[0], 
			mCharacterInstance1.FinalTransforms.size());

		animatedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		for(UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
		{
			mCharacterInstance1.Model->ModelMesh.Draw(md3dImmediateContext, subset);
		}

		// Instance 2

		world = XMLoadFloat4x4(&mCharacterInstance2.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldView     = world*view;
		worldInvTransposeView = worldInvTranspose*view;
		worldViewProj = world*view*proj;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
		Effects::SsaoNormalDepthFX->SetBoneTransforms(
			&mCharacterInstance2.FinalTransforms[0], 
			mCharacterInstance2.FinalTransforms.size());

		animatedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		for(UINT subset = 0; subset < mCharacterInstance2.Model->SubsetCount; ++subset)
		{
			mCharacterInstance2.Model->ModelMesh.Draw(md3dImmediateContext, subset);
		}


		//world = XMLoadFloat4x4(&mCharacterInstanceAss.World);
		//worldInvTranspose = MathHelper::InverseTranspose(world);
		//worldView = world * view;
		//worldInvTransposeView = worldInvTranspose * view;
		//worldViewProj = world * view * proj;

		//Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		//Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		//Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		//Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
		//Effects::SsaoNormalDepthFX->SetBoneTransforms(
		//	&mCharacterInstanceAss.FinalTransforms[0],
		//	mCharacterInstanceAss.FinalTransforms.size());

		//animatedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//for (UINT subset = 0; subset < 1; ++subset)
		//{
		//	mCharacterInstanceAss.Model->AssModelMesh.Draw(md3dImmediateContext, animatedTech, false);
		//}

		world = XMLoadFloat4x4(&mAssWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;
		worldView = world * view;
		worldInvTransposeView = worldInvTranspose * view;

		Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());



		Effects::SsaoNormalDepthFX->SetBoneTransforms(
			&transs[0],
			transs.size());
		//Effects::NormalMapFX->SetMaterial(mModelInstances[2].Model->Mat[0]);
		//Effects::NormalMapFX->SetDiffuseMap(mModelInstances[2].Model->DiffuseMapSRV[0]);
		//Effects::NormalMapFX->SetNormalMap(mModelInstances[2].Model->NormalMapSRV[0]);

		for (UINT subset = 0; subset < mSkelAssimpModel->m_Entries.size(); ++subset)
		{
			Effects::NormalMapFX->SetMaterial(mAssimpModel->meshes[subset].Mat[0]);
			//Effects::NormalMapFX->SetDiffuseMap(mAssimpModel->meshes[subset].DiffuseMapSRV[0]);
			if (mSkelAssimpModel->m_Entries[subset].NormalMapSRV.empty() == false)
			{
				Effects::NormalMapFX->SetNormalMap(mSkelAssimpModel->m_Entries[subset].NormalMapSRV[0]);
			}
			animatedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			mSkelAssimpModel->m_Entries[subset].Draw(md3dImmediateContext, animatedTech, false);
		}

		//world = XMLoadFloat4x4(&mAssSkelWorld);
		//worldInvTranspose = MathHelper::InverseTranspose(world);
		//worldViewProj = world * view * proj;
		//worldView = world * view;
		//worldInvTransposeView = worldInvTranspose * view;

		//Effects::SsaoNormalDepthFX->SetWorldView(worldView);
		//Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
		//Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
		//Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());



		//Effects::SsaoNormalDepthFX->SetBoneTransforms(
		//	&transskel[0],
		//	transskel.size());
		////Effects::NormalMapFX->SetMaterial(mModelInstances[2].Model->Mat[0]);
		////Effects::NormalMapFX->SetDiffuseMap(mModelInstances[2].Model->DiffuseMapSRV[0]);
		////Effects::NormalMapFX->SetNormalMap(mModelInstances[2].Model->NormalMapSRV[0]);

		//for (UINT subset = 0; subset < 1; ++subset)
		//{
		//	//Effects::NormalMapFX->SetMaterial(mAssimpModel->meshes[subset].Mat[0]);
		//	//Effects::NormalMapFX->SetDiffuseMap(mAssimpModel->meshes[subset].DiffuseMapSRV[0]);
		//	if (animod.getModel().NormalMapSRV.empty() == false)
		//	{
		//		Effects::NormalMapFX->SetNormalMap(animod.getModel().NormalMapSRV[0]);
		//	}
		//	animatedTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//	animod.getModel().Draw(md3dImmediateContext, animatedTech, false);
		//}
	}
	//md3dImmediateContext->RSSetState(0);
	//float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	//mFire.SetEyePos(mCam.GetPosition());
	//mFire.Draw(md3dImmediateContext, mCam);
	mTerrain.Draw(md3dImmediateContext, mCam, mDirLights);
	//terr
	//md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	//md3dImmediateContext->IASetInputLayout(InputLayouts::Terrain);

	//stride = sizeof(Vertex::Terrain);
	//offset = 0;

	//md3dImmediateContext->IASetVertexBuffers(0, 1, &mTerrain.mQuadPatchVB, &stride, &offset);
	//md3dImmediateContext->IASetIndexBuffer(mTerrain.mQuadPatchIB, DXGI_FORMAT_R16_UINT, 0);

	//XMFLOAT4 worldPlanes[6];
	//ExtractFrustumPlanes(worldPlanes, viewProj);

	//Effects::TerrainFX->SetViewProj(viewProj);
	//Effects::TerrainFX->Setview(mCam.View());
	//Effects::TerrainFX->SetEyePosW(mCam.GetPosition());

	//Effects::TerrainFX->SetMinDist(20.0f);
	//Effects::TerrainFX->SetMaxDist(500.0f);
	//Effects::TerrainFX->SetMinTess(0.0f);
	//Effects::TerrainFX->SetMaxTess(6.0f);
	//Effects::TerrainFX->SetTexelCellSpaceU(1.0f / mTerrain.mInfo.HeightmapWidth);
	//Effects::TerrainFX->SetTexelCellSpaceV(1.0f / mTerrain.mInfo.HeightmapHeight);
	//Effects::TerrainFX->SetWorldCellSpace(mTerrain.mInfo.CellSpacing);
	//Effects::TerrainFX->SetWorldFrustumPlanes(worldPlanes);

	//Effects::TerrainFX->SetHeightMap(mTerrain.mHeightMapSRV);

	//ID3DX11EffectTechnique* techi = Effects::TerrainFX->NormalDepthTech;
	////D3DX11_TECHNIQUE_DESC techDesc;
	//techi->GetDesc(&techDesc);

	//for (int p = 0; p < techDesc.Passes; p++) {
	//	ID3DX11EffectPass* pass = techi->GetPassByIndex(p);
	//	pass->Apply(0, md3dImmediateContext);

	//	md3dImmediateContext->DrawIndexed(mTerrain.mNumPatchQuadFaces * 4, 0, 0);
	//}
	//md3dImmediateContext->HSSetShader(0, 0, 0);
	//md3dImmediateContext->DSSetShader(0, 0, 0);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//terrend



	md3dImmediateContext->RSSetState(0);
}

void SkinnedMeshApp::DrawSceneToShadowMap()
{
	XMMATRIX view     = XMLoadFloat4x4(&mLightView);
	XMMATRIX proj     = XMLoadFloat4x4(&mLightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	Effects::BuildShadowMapFX->SetEyePosW(mCam.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	// These properties could be set per object if needed.
	Effects::BuildShadowMapFX->SetHeightScale(0.07f);
	Effects::BuildShadowMapFX->SetMaxTessDistance(1.0f);
	Effects::BuildShadowMapFX->SetMinTessDistance(25.0f);
	Effects::BuildShadowMapFX->SetMinTessFactor(1.0f);
	Effects::BuildShadowMapFX->SetMaxTessFactor(5.0f);

	ID3DX11EffectTechnique* smapTech = Effects::BuildShadowMapFX->BuildShadowMapTech;
	ID3DX11EffectTechnique* animatedSmapTech = Effects::BuildShadowMapFX->BuildShadowMapSkinnedTech;

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//mTerrain.DrawShadow(md3dImmediateContext, mCam, mDirLights, viewProj);


	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	//
	// Draw the grid, cylinders, spheres, and box.
	// 


     
	if( GetAsyncKeyState('1') & 0x8000 )
		md3dImmediateContext->RSSetState(RenderStates::WireframeRS);
	
    D3DX11_TECHNIQUE_DESC techDesc;
	
	UINT stride = sizeof(Vertex::Basic32);
	UINT offset = 0;

	smapTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		auto rview = scn.registry.view<component::Shape, component::Transform>();
		for (auto entity : rview)
		{
			auto& shap = rview.get<component::Shape>(entity);
			auto& trans = rview.get<component::Transform>(entity);
			if (scn.registry.has<component::RigidPhysics>(entity) && scn.registry.get<component::RigidPhysics>(entity).enablePhx)
			{
				//scn.physic[entt::to_integral<entt::entity>(entity)]

				auto search = scn.physic.find(entt::to_integral<entt::entity>(entity));

				if (search != scn.physic.end())
				{

					XMFLOAT4X4 tmp;
					const PxMat44 shapePose = search->second->getGlobalPose();
					tmp._11 = shapePose.column0.x;
					tmp._12 = shapePose.column0.y;
					tmp._13 = shapePose.column0.z;
					tmp._14 = shapePose.column0.w;

					tmp._21 = shapePose.column1.x;
					tmp._22 = shapePose.column1.y;
					tmp._23 = shapePose.column1.z;
					tmp._24 = shapePose.column1.w;

					tmp._31 = shapePose.column2.x;
					tmp._32 = shapePose.column2.y;
					tmp._33 = shapePose.column2.z;
					tmp._34 = shapePose.column2.w;

					tmp._41 = shapePose.column3.x;
					tmp._42 = shapePose.column3.y;
					tmp._43 = shapePose.column3.z;
					tmp._44 = shapePose.column3.w;

					trans.Transform = tmp;
				}
			}

			md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
			md3dImmediateContext->IASetVertexBuffers(0, 1, &shap.ShapeVB, &stride, &offset);
			md3dImmediateContext->IASetIndexBuffer(shap.ShapeIB, DXGI_FORMAT_R32_UINT, 0);
			// Draw the box.
			world = XMLoadFloat4x4(&trans.Transform);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));

			smapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(36, 0, 0);
		}
	}

	stride = sizeof(Vertex::PosNormalTexTan);
	offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mShapesVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mShapesIB, DXGI_FORMAT_R32_UINT, 0);

    smapTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		// Draw the grid.
		world = XMLoadFloat4x4(&mGridWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(8.0f, 10.0f, 1.0f));

		smapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

		// Draw the box.
		world = XMLoadFloat4x4(&mBoxWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(2.0f, 1.0f, 1.0f));

		smapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

		// Draw the cylinders.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mCylWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));

			smapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// draw bullets
		for (size_t i = 0; i < bulletVector.size(); i++)
		{

			// Set per object constants.
			XMFLOAT4X4 tmp;
			const PxMat44 shapePose = bulletVector[i]->getGlobalPose();
			tmp._11 = shapePose.column0.x;
			tmp._12 = shapePose.column0.y;
			tmp._13 = shapePose.column0.z;
			tmp._14 = shapePose.column0.w;

			tmp._21 = shapePose.column1.x;
			tmp._22 = shapePose.column1.y;
			tmp._23 = shapePose.column1.z;
			tmp._24 = shapePose.column1.w;

			tmp._31 = shapePose.column2.x;
			tmp._32 = shapePose.column2.y;
			tmp._33 = shapePose.column2.z;
			tmp._34 = shapePose.column2.w;

			tmp._41 = shapePose.column3.x;
			tmp._42 = shapePose.column3.y;
			tmp._43 = shapePose.column3.z;
			tmp._44 = shapePose.column3.w;

			world = XMLoadFloat4x4(&tmp);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world * view * proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 2.0f, 1.0f));

			smapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
		}

		// Draw the spheres.
		for(int i = 0; i < 10; ++i)
		{
			world = XMLoadFloat4x4(&mSphereWorld[i]);
			worldInvTranspose = MathHelper::InverseTranspose(world);
			worldViewProj = world*view*proj;

			Effects::BuildShadowMapFX->SetWorld(world);
			Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

			smapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
			md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
		}

		world = XMLoadFloat4x4(&mAssWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		for (UINT subset = 0; subset < mAssimpModel->meshes.size(); ++subset)
		{
			//Effects::NormalMapFX->SetMaterial(mAssimpModel->Mat[subset]);
			//Effects::BuildShadowMapFX->SetDiffuseMap(mAssimpModel->meshes[subset].DiffuseMapSRV[0]);
			//Effects::NormalMapFX->SetNormalMap(mAssimpModel->NormalMapSRV[subset]);
			smapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			mAssimpModel->meshes[subset].Draw(md3dImmediateContext, smapTech, false);
		}
    }

	//
	// Draw the skull.
	//
	stride = sizeof(Vertex::Basic32);
    offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mSkullVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mSkullIB, DXGI_FORMAT_R32_UINT, 0);

	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		world = XMLoadFloat4x4(&mSkullWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());

		smapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(mSkullIndexCount, 0, 0);
	}

	//terr
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	md3dImmediateContext->IASetInputLayout(InputLayouts::Terrain);

	stride = sizeof(Vertex::Terrain);
	offset = 0;

	md3dImmediateContext->IASetVertexBuffers(0, 1, &mTerrain.mQuadPatchVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mTerrain.mQuadPatchIB, DXGI_FORMAT_R16_UINT, 0);

	XMFLOAT4 worldPlanes[6];
	ExtractFrustumPlanes(worldPlanes, viewProj);

	Effects::TerrainFX->SetViewProj(viewProj);
	Effects::TerrainFX->SetEyePosW(mCam.GetPosition());

	Effects::TerrainFX->SetMinDist(20.0f);
	Effects::TerrainFX->SetMaxDist(500.0f);
	Effects::TerrainFX->SetMinTess(0.0f);
	Effects::TerrainFX->SetMaxTess(6.0f);

	Effects::TerrainFX->SetWorldCellSpace(mTerrain.mInfo.CellSpacing);
	Effects::TerrainFX->SetWorldFrustumPlanes(worldPlanes);

	Effects::TerrainFX->SetHeightMap(mTerrain.mHeightMapSRV);

	ID3DX11EffectTechnique* tech = Effects::TerrainFX->TessBuildShadowMapTech;
	//D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);

	for (int p = 0; p < techDesc.Passes; p++) {
		ID3DX11EffectPass* pass = tech->GetPassByIndex(p);
		pass->Apply(0, md3dImmediateContext);

		md3dImmediateContext->DrawIndexed(mTerrain.mNumPatchQuadFaces * 4, 0, 0);
	}
	md3dImmediateContext->HSSetShader(0, 0, 0);
	md3dImmediateContext->DSSetShader(0, 0, 0);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//terrend

	//
	// Draw the animated characters.
	//

	md3dImmediateContext->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);

	animatedSmapTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		// Instance 1

		world = XMLoadFloat4x4(&mCharacterInstance1.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());
		Effects::BuildShadowMapFX->SetBoneTransforms(
			&mCharacterInstance1.FinalTransforms[0], 
			mCharacterInstance1.FinalTransforms.size());


		animatedSmapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		for(UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
		{
			mCharacterInstance1.Model->ModelMesh.Draw(md3dImmediateContext, subset);
		}

		// Instance 2

		world = XMLoadFloat4x4(&mCharacterInstance2.World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());
		Effects::BuildShadowMapFX->SetBoneTransforms(
			&mCharacterInstance2.FinalTransforms[0], 
			mCharacterInstance2.FinalTransforms.size());

		animatedSmapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		for(UINT subset = 0; subset < mCharacterInstance1.Model->SubsetCount; ++subset)
		{
			mCharacterInstance2.Model->ModelMesh.Draw(md3dImmediateContext, subset);
		}

		//world = XMLoadFloat4x4(&mCharacterInstanceAss.World);
		//worldInvTranspose = MathHelper::InverseTranspose(world);
		//worldViewProj = world * view * proj;

		//Effects::BuildShadowMapFX->SetWorld(world);
		//Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		//Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		//Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());
		//Effects::BuildShadowMapFX->SetBoneTransforms(
		//	&mCharacterInstanceAss.FinalTransforms[0],
		//	mCharacterInstanceAss.FinalTransforms.size());

		//animatedSmapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//for (UINT subset = 0; subset < 1; ++subset)
		//{
		//	mCharacterInstanceAss.Model->AssModelMesh.Draw(md3dImmediateContext, animatedSmapTech, false);
		//}



		world = XMLoadFloat4x4(&mAssWorld);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world * view * proj;

		Effects::BuildShadowMapFX->SetWorld(world);
		Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());


		Effects::BuildShadowMapFX->SetBoneTransforms(
			&transs[0],
			transs.size());
		//Effects::NormalMapFX->SetMaterial(mModelInstances[2].Model->Mat[0]);
		//Effects::NormalMapFX->SetDiffuseMap(mModelInstances[2].Model->DiffuseMapSRV[0]);
		//Effects::NormalMapFX->SetNormalMap(mModelInstances[2].Model->NormalMapSRV[0]);

		for (UINT subset = 0; subset < mSkelAssimpModel->m_Entries.size(); ++subset)
		{
			//Effects::NormalMapFX->SetMaterial(mAssimpModel->meshes[subset].Mat[0]);
			//Effects::NormalMapFX->SetDiffuseMap(mAssimpModel->meshes[subset].DiffuseMapSRV[0]);
			if (mSkelAssimpModel->m_Entries[subset].NormalMapSRV.empty() == false)
			{
				Effects::NormalMapFX->SetNormalMap(mSkelAssimpModel->m_Entries[subset].NormalMapSRV[0]);
			}
			animatedSmapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

			mSkelAssimpModel->m_Entries[subset].Draw(md3dImmediateContext, animatedSmapTech, false);
		}
		// 
		//world = XMLoadFloat4x4(&mAssSkelWorld);
		//worldInvTranspose = MathHelper::InverseTranspose(world);
		//worldViewProj = world * view * proj;

		//Effects::BuildShadowMapFX->SetWorld(world);
		//Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
		//Effects::BuildShadowMapFX->SetWorldViewProj(worldViewProj);
		//Effects::BuildShadowMapFX->SetTexTransform(XMMatrixIdentity());


		//Effects::BuildShadowMapFX->SetBoneTransforms(
		//	&transskel[0],
		//	transskel.size());
		////Effects::NormalMapFX->SetMaterial(mModelInstances[2].Model->Mat[0]);
		////Effects::NormalMapFX->SetDiffuseMap(mModelInstances[2].Model->DiffuseMapSRV[0]);
		////Effects::NormalMapFX->SetNormalMap(mModelInstances[2].Model->NormalMapSRV[0]);

		//for (UINT subset = 0; subset < 1; ++subset)
		//{
		//	//Effects::NormalMapFX->SetMaterial(mAssimpModel->meshes[subset].Mat[0]);
		//	//Effects::NormalMapFX->SetDiffuseMap(mAssimpModel->meshes[subset].DiffuseMapSRV[0]);
		//	if (animod.getModel().NormalMapSRV.empty() == false)
		//	{
		//		Effects::NormalMapFX->SetNormalMap(animod.getModel().NormalMapSRV[0]);
		//	}
		//	animatedSmapTech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);

		//	animod.getModel().Draw(md3dImmediateContext, animatedSmapTech, false);
		//}
	}
	//md3dImmediateContext->RSSetState(0);
	//float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//md3dImmediateContext->OMSetBlendState(0, blendFactor, 0xffffffff); // restore default
	//mFire.SetEyePos(mCam.GetPosition());
	//mFire.Draw(md3dImmediateContext, mCam);
	md3dImmediateContext->RSSetState(0);
}

void SkinnedMeshApp::DrawScreenQuad(ID3D11ShaderResourceView* srv)
{
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R32_UINT, 0);
 
	// Scale and shift quad to lower-right corner.
	XMMATRIX world(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f);

	ID3DX11EffectTechnique* tech = Effects::DebugTexFX->ViewRedTech;
	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		Effects::DebugTexFX->SetWorldViewProj(world);
		Effects::DebugTexFX->SetTexture(srv);

		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(6, 0, 0);
    }
}

void SkinnedMeshApp::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMVECTOR lightDir = XMLoadFloat3(&mDirLights[0].Direction);
	XMVECTOR lightPos = -2.0f*mSceneBounds.Radius*lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&mLightView, V);
	XMStoreFloat4x4(&mLightProj, P);
	XMStoreFloat4x4(&mShadowTransform, S);
}

void SkinnedMeshApp::BuildShapeGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);
	geoGen.CreateGrid(20.0f, 30.0f, 50, 40, grid);
	geoGen.CreateSphere(0.5f, 20, 20, sphere);
	geoGen.CreateCylinder(0.5f, 0.5f, 3.0f, 15, 15, cylinder);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	mBoxVertexOffset      = 0;
	mGridVertexOffset     = box.Vertices.size();
	mSphereVertexOffset   = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	// Cache the index count of each object.
	mBoxIndexCount      = box.Indices.size();
	mGridIndexCount     = grid.Indices.size();
	mSphereIndexCount   = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	mBoxIndexOffset      = 0;
	mGridIndexOffset     = mBoxIndexCount;
	mSphereIndexOffset   = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;
	
	UINT totalVertexCount = 
		box.Vertices.size() + 
		grid.Vertices.size() + 
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	UINT totalIndexCount = 
		mBoxIndexCount + 
		mGridIndexCount + 
		mSphereIndexCount +
		mCylinderIndexCount;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::PosNormalTexTan> vertices(totalVertexCount);

	UINT k = 0;
	for(size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos      = box.Vertices[i].Position;
		vertices[k].Normal   = box.Vertices[i].Normal;
		vertices[k].Tex      = box.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			box.Vertices[i].TangentU.x,
			box.Vertices[i].TangentU.y,
			box.Vertices[i].TangentU.z,
			1.0f);
	}

	for(size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos      = grid.Vertices[i].Position;
		vertices[k].Normal   = grid.Vertices[i].Normal;
		vertices[k].Tex      = grid.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			grid.Vertices[i].TangentU.x,
			grid.Vertices[i].TangentU.y,
			grid.Vertices[i].TangentU.z,
			1.0f);
	}

	for(size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos      = sphere.Vertices[i].Position;
		vertices[k].Normal   = sphere.Vertices[i].Normal;
		vertices[k].Tex      = sphere.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			sphere.Vertices[i].TangentU.x,
			sphere.Vertices[i].TangentU.y,
			sphere.Vertices[i].TangentU.z,
			1.0f);
	}

	for(size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos      = cylinder.Vertices[i].Position;
		vertices[k].Normal   = cylinder.Vertices[i].Normal;
		vertices[k].Tex      = cylinder.Vertices[i].TexC;
		vertices[k].TangentU = XMFLOAT4(
			cylinder.Vertices[i].TangentU.x,
			cylinder.Vertices[i].TangentU.y,
			cylinder.Vertices[i].TangentU.z,
			1.0f);
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosNormalTexTan) * totalVertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mShapesVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mShapesIB));
}
 
void SkinnedMeshApp::BuildSkullGeometryBuffers()
{
	std::ifstream fin("Models/skull.txt");
	
	if(!fin)
	{
		MessageBox(0, L"Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0;
	UINT tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;
	
	std::vector<Vertex::Basic32> vertices(vcount);
	for(UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	mSkullIndexCount = 3*tcount;
	std::vector<UINT> indices(mSkullIndexCount);
	for(UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i*3+0] >> indices[i*3+1] >> indices[i*3+2];
	}

	fin.close();

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * vcount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mSkullVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * mSkullIndexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mSkullIB));
}

void SkinnedMeshApp::BuildScreenQuadGeometryBuffers()
{
	GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(quad.Vertices.size());

	for(UINT i = 0; i < quad.Vertices.size(); ++i)
	{
		vertices[i].Pos    = quad.Vertices[i].Position;
		vertices[i].Normal = quad.Vertices[i].Normal;
		vertices[i].Tex    = quad.Vertices[i].TexC;
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * quad.Vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mScreenQuadVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * quad.Indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &quad.Indices[0];
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mScreenQuadIB));
}
