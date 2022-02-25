#pragma once
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui/imgui.h"
#include "imgui/imguizmo/ImGuizmo.h"
#include "imgui/imgui/imgui_internal.h"
#include  <entt/entt.hpp>
#include  "Components.h"
#include "Camera.h"
#include "Physics.h"


class Scene
{
public:
	void ShowEntityList(const Camera& Cam);
	void Init(ID3D11Device* md3dDevice, std::shared_ptr<Physics> phx);
	entt::registry registry;
	std::map<int, PxRigidDynamic*> physic;
	Scene();
private:
	void ShowComponents(const Camera& Cam);
	void BuildCrateGeometryBuffers();
	void ShowGuizmo(const Camera& Cam);
	void EditTransform(float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition);
	ID3D11Device* md3dDevice;
	std::shared_ptr<Physics> phx;
	int selected;
	bool mShowComponents;
	bool useWindow;
	ImGuizmo::OPERATION mCurrentGizmoOperation;
	float camDistance;
};

