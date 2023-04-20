#include "Scene.h"
#include <DirectXMath.h>
#include <GeometryGenerator.h>

void Scene::ShowEntityList(const Camera& Cam)
{
    ImGui::Begin("Entity List");

    if (ImGui::Button("Add Entity"))
    {
        const auto entity = registry.create();
        registry.emplace<component::Name>(entity, "New Entity");
    }

    if (ImGui::TreeNode("Entitys"))
    {

        auto view = registry.view<component::Name>();

        for (auto entity : view) {
            auto& name = view.get<component::Name>(entity);
            
            std::string tmp = name.name + " " + std::to_string(static_cast<int>(entity));
            char* buf=tmp.data();
            //sprintf(buf, "%d %d", name.name.c_str(), entity);
            if (ImGui::Selectable(buf, selected == static_cast<int>(entity)))
            {
                selected = static_cast<int>(entity);
                mShowComponents = true;
            }
        }
        ImGui::TreePop();
    }

    if (mShowComponents)
    {
        ShowComponents(Cam);
    }
   
    ImGui::End();
}

void Scene::ShowComponents(const Camera& Cam)
{
    auto& view = registry.get<component::Name>(static_cast<entt::entity>(selected));
    //auto vienw = registry.set<component::Name>(static_cast<entt::entity>(selected));

	ImGuizmo::BeginFrame();
    ImGui::Begin("Components");


    if (ImGui::Button("+ Add Component")) {
        ImGui::OpenPopup("Add Component");
    }

    if (ImGui::BeginPopup("Add Component")) {
        ImGui::TextUnformatted("Available:");
        ImGui::Separator();
        if (!registry.any_of<component::Name>(static_cast<entt::entity>(selected)))
        {
            if (ImGui::Selectable(component::components[0])) {
                registry.emplace<component::Name>(static_cast<entt::entity>(selected), "New Entity");
            }
        }
        if (!registry.any_of<component::Shape>(static_cast<entt::entity>(selected)))
        {
            if (ImGui::Selectable(component::components[1])) {
                registry.emplace<component::Shape>(static_cast<entt::entity>(selected));
				BuildCrateGeometryBuffers();
            }
        }
        if (!registry.any_of<component::Transform>(static_cast<entt::entity>(selected)))
        {
            if (ImGui::Selectable(component::components[2])) {
                DirectX::XMFLOAT4X4 tmp;
                DirectX::XMStoreFloat4x4(&tmp, DirectX::XMMatrixIdentity());
                
                registry.emplace<component::Transform>(static_cast<entt::entity>(selected), tmp);
            }
        }
		if (!registry.any_of<component::StaticModel>(static_cast<entt::entity>(selected)))
		{
			if (ImGui::Selectable(component::components[3])) {
				AssimpModel mAssimpModel;
				mAssimpModel.Load(md3dDevice, md3dImmediateContext, mTexMgr, "Models\\Wolf\\Wolf_dae.dae", L"Models\\Wolf\\textures\\");

				registry.emplace<component::StaticModel>(static_cast<entt::entity>(selected), mAssimpModel);
			}
		}
	/*	if (!registry.any_of<component::SkeletalModel>(static_cast<entt::entity>(selected)))
		{
			if (ImGui::Selectable(component::components[5])) {
				SkinnedMesh mModel;

				registry.emplace<component::SkeletalModel>(static_cast<entt::entity>(selected), mModel);
			}
		}*/
		//if (!registry.has<component::Cskinnedmesh>(static_cast<entt::entity>(selected)))
		//{
		//	if (ImGui::Selectable(component::components[3])) {
		//		DirectX::XMFLOAT4X4 tmp;
		//		DirectX::XMStoreFloat4x4(&tmp, DirectX::XMMatrixIdentity());

		//		registry.emplace<component::Transform>(static_cast<entt::entity>(selected), "New Entity");
		//	}
		//}
		if (!registry.any_of<component::RigidPhysics>(static_cast<entt::entity>(selected)))
		{
			if (ImGui::Selectable(component::components[4])) {
				PxRigidDynamic* prd;
				auto trans=registry.get<component::Transform>(static_cast<entt::entity>(selected));
				DirectX::XMVECTOR tr;
				DirectX::XMVECTOR rot;
				DirectX::XMVECTOR scal;
				DirectX::XMMatrixDecompose(&scal,&rot,&tr,DirectX::XMLoadFloat4x4(&trans.Transform));

				DirectX::XMFLOAT3 xtr;
				DirectX::XMStoreFloat3(&xtr, tr);


				prd = phx.get()->createDynamic(PxTransform(PxVec3(xtr.x, xtr.y, xtr.z)), PxSphereGeometry(1), PxVec3(0, 0, 0));
				physic.emplace(std::pair<int, PxRigidDynamic*>(selected, prd));
				registry.emplace<component::RigidPhysics>(static_cast<entt::entity>(selected),selected,true);
			}
		}
        ImGui::EndPopup();
    }

    if (registry.any_of<component::Name>(static_cast<entt::entity>(selected)))
    {
        if (ImGui::CollapsingHeader("Name"))
        {
            static char buf1[64];
            auto na = registry.get<component::Name>(static_cast<entt::entity>(selected));

            ImGui::InputText(na.name.c_str(), buf1, IM_ARRAYSIZE(buf1));

            if (ImGui::Button("Save")) 
            {
                auto& sna = registry.get<component::Name>(static_cast<entt::entity>(selected));
                sna.name = buf1;
            }
        }
    }

    if (registry.any_of<component::Shape>(static_cast<entt::entity>(selected)))
    {
        if (ImGui::CollapsingHeader("Shape"))
        {
            //view.name.c_str();
   /*         static char buf1[] = "";
            ImGui::InputText(view.name.c_str(), buf1, 64);*/

        }
    }

    if (registry.any_of<component::Transform>(static_cast<entt::entity>(selected)))
    {
        if (ImGui::CollapsingHeader("Transform"))
        {
			ShowGuizmo(Cam);
        }
    }

	if (registry.any_of<component::StaticModel>(static_cast<entt::entity>(selected)))
	{
		if (ImGui::CollapsingHeader("Static Model"))
		{
			static char str0[128];
			ImGui::InputText("Model", str0, IM_ARRAYSIZE(str0));
			static char str1[128];
			ImGui::InputText("Texture", str1, IM_ARRAYSIZE(str1));

			if (ImGui::Button("Save"))
			{
				std::string modelp(str0);
				std::string texturep(str1);
				std::wstring wstexturep(texturep.begin(), texturep.end());
				AssimpModel mAssimpModel;
				mAssimpModel.Load(md3dDevice, md3dImmediateContext, mTexMgr, modelp, wstexturep);


				auto& mo = registry.get<component::StaticModel>(static_cast<entt::entity>(selected));
				mo.model = mAssimpModel;
				//registry.emplace<component::SkeletalModel>(static_cast<entt::entity>(selected), mAssimpModel);
			}
		}
	}

	//if (registry.any_of<component::SkeletalModel>(static_cast<entt::entity>(selected)))
	//{
	//	if (ImGui::CollapsingHeader("Skeletal Model"))
	//	{
	//		static char str0[128];
	//		ImGui::InputText("Model", str0, IM_ARRAYSIZE(str0));
	//		static char str1[128];
	//		ImGui::InputText("Texture", str1, IM_ARRAYSIZE(str1));

	//		if (ImGui::Button("Save"))
	//		{
	//			std::string modelp(str0);
	//			std::string texturep(str1);
	//			std::wstring wstexturep(texturep.begin(), texturep.end());

	//			auto& mo = registry.get<component::SkeletalModel>(static_cast<entt::entity>(selected));
	//			mo.skelmodel.LoadMesh(md3dDevice, md3dImmediateContext, mTexMgr, modelp, wstexturep);
	//			//registry.emplace<component::SkeletalModel>(static_cast<entt::entity>(selected), mAssimpModel);
	//		}
	//	}
	//}
	//if (registry.has<component::Cskinnedmesh>(static_cast<entt::entity>(selected)))
	//{
	//	if (ImGui::CollapsingHeader("SkinnedMesh"))
	//	{
	//	}
	//}

	if (registry.any_of<component::RigidPhysics>(static_cast<entt::entity>(selected)))
	{
		if (ImGui::CollapsingHeader("Physics"))
		{
			auto& phx = registry.get<component::RigidPhysics>(static_cast<entt::entity>(selected));
			ImGui::Checkbox("Enable Physics", &phx.enablePhx);
		}
	}

    ImGui::End();
}

Scene::Scene()
{
	useWindow=false;
    selected = -1;
    mShowComponents = false;
    mCurrentGizmoOperation=ImGuizmo::TRANSLATE;
    camDistance = 8.f;
}

void Scene::Init(ID3D11Device* Md3dDevice, ID3D11DeviceContext* Md3dImmediateContext, TextureMgr* TexMgr, std::shared_ptr<Physics> Phx)
{
	md3dDevice = Md3dDevice;
	md3dImmediateContext = Md3dImmediateContext;
	mTexMgr= TexMgr;
	phx = Phx;
}



void Scene::EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
	static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f };
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	if (editTransformDecomposition)
	{
		if (ImGui::IsKeyPressed(90))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Universal", mCurrentGizmoOperation == ImGuizmo::UNIVERSAL))
			mCurrentGizmoOperation = ImGuizmo::UNIVERSAL;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Tr", matrixTranslation);
		ImGui::InputFloat3("Rt", matrixRotation);
		ImGui::InputFloat3("Sc", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}
		if (ImGui::IsKeyPressed(83))
			useSnap = !useSnap;
		//ImGui::Checkbox("", &useSnap);
		ImGui::SameLine();

		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}
		ImGui::Checkbox("Bound Sizing", &boundSizing);
		if (boundSizing)
		{
			ImGui::PushID(3);
			ImGui::Checkbox("", &boundSizingSnap);
			ImGui::SameLine();
			ImGui::InputFloat3("Snap", boundsSnap);
			ImGui::PopID();
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;
	if (useWindow)
	{
		ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
		ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
		ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
		viewManipulateTop = ImGui::GetWindowPos().y;
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
	}
	else
	{
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	}

	ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
	//ImGuizmo::DrawCubes(cameraView, cameraProjection, matrix, 1);//gizmoCount
	ImGuizmo::Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

	ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

	if (useWindow)
	{
		ImGui::End();
		ImGui::PopStyleColor(1);
	}
}

void Scene::ShowGuizmo(const Camera& Cam)
{

	if (ImGui::RadioButton("Full view", !useWindow)) useWindow = false;
	ImGui::SameLine();
	if (ImGui::RadioButton("Window", useWindow)) useWindow = true;

	/*ImGui::Text("Camera");
	bool viewDirty = false;
	if (ImGui::RadioButton("Perspective", isPerspective)) isPerspective = true;
	ImGui::SameLine();
	if (ImGui::RadioButton("Orthographic", !isPerspective)) isPerspective = false;
	if (isPerspective)
	{
		ImGui::SliderFloat("Fov", &fov, 20.f, 110.f);
	}
	else
	{
		ImGui::SliderFloat("Ortho width", &viewWidth, 1, 20);
	}
	viewDirty |= ImGui::SliderFloat("Distance", &camDistance, 1.f, 10.f);
	ImGui::SliderInt("Gizmo count", &gizmoCount, 1, 4);*/

	//if (viewDirty || firstFrame)
	//{
	//	float eye[] = { cosf(camYAngle) * cosf(camXAngle) * camDistance, sinf(camXAngle) * camDistance, sinf(camYAngle) * cosf(camXAngle) * camDistance };
	//	float at[] = { 0.f, 0.f, 0.f };
	//	float up[] = { 0.f, 1.f, 0.f };
	//	LookAt(eye, at, up, cameraView);
	//	mCam.View();
	//	firstFrame = false;
	//}

	//ImGui::Text("X: %f Y: %f", mLastMousePos.x, mLastMousePos.y);
	if (ImGuizmo::IsUsing())
	{
		ImGui::Text("Using gizmo");
	}
	else
	{
		ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
	}
	ImGui::Separator();
	for (int matId = 0; matId < 1; matId++)
	{
		ImGuizmo::SetID(matId);

		DirectX::XMFLOAT4X4 tmp1;
		DirectX::XMFLOAT4X4 tmp2;
		auto& s = registry.get<component::Transform>(static_cast<entt::entity>(selected));
		DirectX::XMFLOAT4X4 mBoxWorldguizmo=s.Transform;
		XMStoreFloat4x4(&tmp1, Cam.View());
		XMStoreFloat4x4(&tmp2, Cam.Proj());

		float cameraView[16] =
		{ tmp1._11, tmp1._12, tmp1._13, tmp1._14,
		  tmp1._21, tmp1._22, tmp1._23, tmp1._24,
		  tmp1._31, tmp1._32, tmp1._33, tmp1._34,
		  tmp1._41, tmp1._42, tmp1._43, tmp1._44 };

		float cameraProjection[16] =
		{ tmp2._11, tmp2._12, tmp2._13, tmp2._14,
		  tmp2._21, tmp2._22, tmp2._23, tmp2._24,
		  tmp2._31, tmp2._32, tmp2._33, tmp2._34,
		  tmp2._41, tmp2._42, tmp2._43, tmp2._44 };

		float BoxWorldguizmo[16] =
		{ mBoxWorldguizmo._11, mBoxWorldguizmo._12, mBoxWorldguizmo._13, mBoxWorldguizmo._14,
		  mBoxWorldguizmo._21, mBoxWorldguizmo._22, mBoxWorldguizmo._23, mBoxWorldguizmo._24,
		  mBoxWorldguizmo._31, mBoxWorldguizmo._32, mBoxWorldguizmo._33, mBoxWorldguizmo._34,
		  mBoxWorldguizmo._41, mBoxWorldguizmo._42, mBoxWorldguizmo._43, mBoxWorldguizmo._44 };


		EditTransform(cameraView, cameraProjection, BoxWorldguizmo, true);//lastUsing == matId
		if (ImGuizmo::IsUsing())
		{
			//lastUsing = matId;
		}
		mBoxWorldguizmo._11 = BoxWorldguizmo[0];
		mBoxWorldguizmo._12 = BoxWorldguizmo[1];
		mBoxWorldguizmo._13 = BoxWorldguizmo[2];
		mBoxWorldguizmo._14 = BoxWorldguizmo[3];

		mBoxWorldguizmo._21 = BoxWorldguizmo[4];
		mBoxWorldguizmo._22 = BoxWorldguizmo[5];
		mBoxWorldguizmo._23 = BoxWorldguizmo[6];
		mBoxWorldguizmo._24 = BoxWorldguizmo[7];

		mBoxWorldguizmo._31 = BoxWorldguizmo[8];
		mBoxWorldguizmo._32 = BoxWorldguizmo[9];
		mBoxWorldguizmo._33 = BoxWorldguizmo[10];
		mBoxWorldguizmo._34 = BoxWorldguizmo[11];

		mBoxWorldguizmo._41 = BoxWorldguizmo[12];
		mBoxWorldguizmo._42 = BoxWorldguizmo[13];
		mBoxWorldguizmo._43 = BoxWorldguizmo[14];
		mBoxWorldguizmo._44 = BoxWorldguizmo[15];
		s.Transform= mBoxWorldguizmo;

	}
}

void Scene::BuildCrateGeometryBuffers()
{
	GeometryGenerator::MeshData box;
	auto& Shape = registry.get<component::Shape>(static_cast<entt::entity>(selected));
	
	Shape.ShapeIB = nullptr;
	Shape.ShapeVB = nullptr;
	GeometryGenerator geoGen;
	geoGen.CreateBox(1.0f, 1.0f, 1.0f, box);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::Basic32> vertices(box.Vertices.size());

	for (UINT i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].Tex = box.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * box.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];
	HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &Shape.ShapeVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * box.Indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &box.Indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &Shape.ShapeIB));
}
