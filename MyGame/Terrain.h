//***************************************************************************************
// Terrain.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Class that renders a terrain using hardware tessellation and multitexturing.
//***************************************************************************************

#ifndef TERRAIN_H
#define TERRAIN_H

#include "d3dUtil.h"

class Camera;
struct DirectionalLight;

class Terrain
{
public:
	struct InitInfo
	{
		std::wstring HeightMapFilename;
		std::wstring LayerMapFilename0;
		std::wstring LayerMapFilename1;
		std::wstring LayerMapFilename2;
		std::wstring LayerMapFilename3;
		std::wstring LayerMapFilename4;
		std::wstring BlendMapFilename;
		float HeightScale;
		UINT HeightmapWidth;
		UINT HeightmapHeight;
		float CellSpacing;
	};

public:
	Terrain();
	~Terrain();

	float GetWidth()const;
	float GetDepth()const;
	float GetHeight(float x, float z)const;

	XMMATRIX GetWorld()const;
	void SetWorld(CXMMATRIX M);

	void Init(ID3D11Device* device, ID3D11DeviceContext* dc, const InitInfo& initInfo);

	void Draw(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	void Draw(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3], ID3D11ShaderResourceView* ssao, ID3D11ShaderResourceView* shadow, CXMMATRIX shadowtrans);
	//void DrawSSAO(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3]);
	//void DrawShadow(ID3D11DeviceContext* dc, const Camera& cam, DirectionalLight lights[3], CXMMATRIX viewProje);

	//public void DrawToShadowMap(DeviceContext dc, ShadowMap sMap, Matrix viewProj) {
	//	sMap.BindDsvAndSetNullRenderTarget(dc);

	//	dc.InputAssembler.PrimitiveTopology = PrimitiveTopology.PatchListWith4ControlPoints;
	//	dc.InputAssembler.InputLayout = InputLayouts.TerrainCP;

	//	var stride = TerrainCP.Stride;
	//	const int offset = 0;

	//	dc.InputAssembler.SetVertexBuffers(0, new VertexBufferBinding(_quadPatchVB, stride, offset));
	//	dc.InputAssembler.SetIndexBuffer(_quadPatchIB, Format.R16_UInt, 0);


	//	var frustum = Frustum.FromViewProj(viewProj);
	//	var planes = frustum.Planes;

	//	Effects.TerrainFX.SetViewProj(viewProj);
	//	Effects.TerrainFX.SetEyePosW(new Vector3(viewProj.M41, viewProj.M42, viewProj.M43));
	//	Effects.TerrainFX.SetMinDist(MinDist);
	//	Effects.TerrainFX.SetMaxDist(MaxDist);
	//	Effects.TerrainFX.SetMinTess(MinTess);
	//	Effects.TerrainFX.SetMaxTess(MaxTess);
	//	Effects.TerrainFX.SetWorldCellSpace(Info.CellSpacing);
	//	Effects.TerrainFX.SetWorldFrustumPlanes(planes);
	//	Effects.TerrainFX.SetHeightMap(_heightMapSRV);

	//	var tech = Effects.TerrainFX.TessBuildShadowMapTech;
	//	for (int p = 0; p < tech.Description.PassCount; p++) {
	//		var pass = tech.GetPassByIndex(p);
	//		pass.Apply(dc);
	//		dc.DrawIndexed(_numPatchQuadFaces * 4, 0, 0);
	//	}
	//	dc.HullShader.Set(null);
	//	dc.DomainShader.Set(null);
	//}

	//public void ComputeSsao(DeviceContext dc, CameraBase cam, Ssao ssao, DepthStencilView depthStencilView) {
	//	ssao.SetNormalDepthRenderTarget(depthStencilView);

	//	dc.InputAssembler.PrimitiveTopology = PrimitiveTopology.PatchListWith4ControlPoints;
	//	dc.InputAssembler.InputLayout = InputLayouts.TerrainCP;

	//	var stride = TerrainCP.Stride;
	//	const int offset = 0;

	//	dc.InputAssembler.SetVertexBuffers(0, new VertexBufferBinding(_quadPatchVB, stride, offset));
	//	dc.InputAssembler.SetIndexBuffer(_quadPatchIB, Format.R16_UInt, 0);

	//	var viewProj = cam.ViewProj;
	//	var planes = cam.FrustumPlanes;

	//	Effects.TerrainFX.SetViewProj(viewProj);
	//	Effects.TerrainFX.SetEyePosW(cam.Position);
	//	Effects.TerrainFX.SetMinDist(MinDist);
	//	Effects.TerrainFX.SetMaxDist(MaxDist);
	//	Effects.TerrainFX.SetMinTess(MinTess);
	//	Effects.TerrainFX.SetMaxTess(MaxTess);
	//	Effects.TerrainFX.SetTexelCellSpaceU(1.0f / Info.HeightMapWidth);
	//	Effects.TerrainFX.SetTexelCellSpaceV(1.0f / Info.HeightMapHeight);
	//	Effects.TerrainFX.SetWorldCellSpace(Info.CellSpacing);
	//	Effects.TerrainFX.SetWorldFrustumPlanes(planes);
	//	Effects.TerrainFX.SetHeightMap(_heightMapSRV);
	//	Effects.TerrainFX.SetView(cam.View);

	//	var tech = Effects.TerrainFX.NormalDepthTech;
	//	for (int p = 0; p < tech.Description.PassCount; p++) {
	//		var pass = tech.GetPassByIndex(p);
	//		pass.Apply(dc);
	//		dc.DrawIndexed(_numPatchQuadFaces * 4, 0, 0);
	//	}
	//	dc.HullShader.Set(null);
	//	dc.DomainShader.Set(null);

	//	ssao.ComputeSsao(cam);
	//	ssao.BlurAmbientMap(4);
	//}

	//void Draw(DeviceContext dc, CameraBase cam, DirectionalLight[] lights) {
	//	if (_useTessellation) {

	//		dc.InputAssembler.PrimitiveTopology = PrimitiveTopology.PatchListWith4ControlPoints;
	//		dc.InputAssembler.InputLayout = InputLayouts.TerrainCP;

	//		var stride = TerrainCP.Stride;
	//		const int offset = 0;

	//		dc.InputAssembler.SetVertexBuffers(0, new VertexBufferBinding(_quadPatchVB, stride, offset));
	//		dc.InputAssembler.SetIndexBuffer(_quadPatchIB, Format.R16_UInt, 0);

	//		var viewProj = cam.ViewProj;
	//		var planes = cam.FrustumPlanes;
	//		var toTexSpace = Matrix.Scaling(0.5f, -0.5f, 1.0f) * Matrix.Translation(0.5f, 0.5f, 0);

	//		Effects.TerrainFX.SetViewProj(viewProj);
	//		Effects.TerrainFX.SetEyePosW(cam.Position);
	//		Effects.TerrainFX.SetDirLights(lights);
	//		Effects.TerrainFX.SetFogColor(Color.Silver);
	//		Effects.TerrainFX.SetFogStart(15.0f);
	//		Effects.TerrainFX.SetFogRange(175.0f);
	//		Effects.TerrainFX.SetMinDist(MinDist);
	//		Effects.TerrainFX.SetMaxDist(MaxDist);
	//		Effects.TerrainFX.SetMinTess(MinTess);
	//		Effects.TerrainFX.SetMaxTess(MaxTess);
	//		Effects.TerrainFX.SetTexelCellSpaceU(1.0f / Info.HeightMapWidth);
	//		Effects.TerrainFX.SetTexelCellSpaceV(1.0f / Info.HeightMapHeight);
	//		Effects.TerrainFX.SetWorldCellSpace(Info.CellSpacing);
	//		Effects.TerrainFX.SetWorldFrustumPlanes(planes);
	//		Effects.TerrainFX.SetLayerMapArray(_layerMapArraySRV);
	//		Effects.TerrainFX.SetBlendMap(_blendMapSRV);
	//		Effects.TerrainFX.SetHeightMap(_heightMapSRV);
	//		Effects.TerrainFX.SetMaterial(_material);
	//		Effects.TerrainFX.SetViewProjTex(viewProj * toTexSpace);

	//		var tech = Shadows ? Effects.TerrainFX.Light1ShadowTech : Effects.TerrainFX.Light1Tech;
	//		for (int p = 0; p < tech.Description.PassCount; p++) {
	//			var pass = tech.GetPassByIndex(p);
	//			pass.Apply(dc);
	//			dc.DrawIndexed(_numPatchQuadFaces * 4, 0, 0);
	//		}
	//		dc.HullShader.Set(null);
	//		dc.DomainShader.Set(null);

	//	}
	//	else { // dx10 code branch}
	//	}

private:
	void LoadHeightmap();
	void Smooth();
	bool InBounds(int i, int j);
	float Average(int i, int j);
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT i, UINT j);
	void BuildQuadPatchVB(ID3D11Device* device);
	void BuildQuadPatchIB(ID3D11Device* device);
	void BuildHeightmapSRV(ID3D11Device* device);

private:
public:

	// Divide heightmap into patches such that each patch has CellsPerPatch cells
	// and CellsPerPatch+1 vertices.  Use 64 so that if we tessellate all the way 
	// to 64, we use all the data from the heightmap.  
	static const int CellsPerPatch = 64;

	ID3D11Buffer* mQuadPatchVB;
	ID3D11Buffer* mQuadPatchIB;

	ID3D11ShaderResourceView* mLayerMapArraySRV;
	ID3D11ShaderResourceView* mBlendMapSRV;
	ID3D11ShaderResourceView* mHeightMapSRV;

	InitInfo mInfo;

	UINT mNumPatchVertices;
	UINT mNumPatchQuadFaces;

	UINT mNumPatchVertRows;
	UINT mNumPatchVertCols;

	XMFLOAT4X4 mWorld;

	Material mMat;

	std::vector<XMFLOAT2> mPatchBoundsY;
	std::vector<float> mHeightmap;
};

#endif // TERRAIN_H