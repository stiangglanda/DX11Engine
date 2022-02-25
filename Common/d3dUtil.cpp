//***************************************************************************************
// d3dUtil.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "d3dUtil.h"
#include "DirectXTex.h"
#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Camera.h"
#include "DirectXMath.h"
//#include "DirectXHelper.h"
using namespace DirectX;

Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	return byteCode;
}

void d3dHelper::CreateTextureViewArrayFromFiles(
	ID3D11Device* device,
	ID3D11DeviceContext* context,
	std::vector<std::wstring>& filenames,
	ID3D11ShaderResourceView** textureViewArray)
{
	auto textureCount = filenames.size();

	// A temporary collection to keep texture resources.
	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> textures(textureCount);

	// Load textures from DDS files.
	for (int i = 0; i < textureCount; ++i)
	{
		HR(
			CreateDDSTextureFromFile(
				device,
				filenames[i].c_str(),
				(ID3D11Resource**)textures[i].GetAddressOf(),
				nullptr
			)
		);
	}

	// Luna: Create the texture array.
	D3D11_TEXTURE2D_DESC textureDesc;
	textures[0]->GetDesc(&textureDesc); // each element in the texture array has the same format and dimensions

	D3D11_TEXTURE2D_DESC arrayDesc;
	arrayDesc.Width = textureDesc.Width;
	arrayDesc.Height = textureDesc.Height;
	arrayDesc.MipLevels = textureDesc.MipLevels;
	arrayDesc.ArraySize = textureCount;
	arrayDesc.Format = textureDesc.Format;
	arrayDesc.SampleDesc.Count = 1;
	arrayDesc.SampleDesc.Quality = 0;
	arrayDesc.Usage = D3D11_USAGE_DEFAULT;
	arrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	arrayDesc.CPUAccessFlags = 0;
	arrayDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> textureArray = nullptr;
	
		HR(device->CreateTexture2D(&arrayDesc, 0, &textureArray));

	// Copy each texture into the elements of the texture array.
	for (UINT texElement = 0; texElement < textureCount; ++texElement)
	{
		for (UINT mipLevel = 0; mipLevel < textureDesc.MipLevels; ++mipLevel)
		{
			const UINT sourceSubresource = D3D11CalcSubresource(mipLevel, 0, textureDesc.MipLevels);
			const UINT destSubresource = D3D11CalcSubresource(mipLevel, texElement, textureDesc.MipLevels);
			context->CopySubresourceRegion(textureArray.Get(), destSubresource, 0, 0, 0, textures[texElement].Get(), sourceSubresource, nullptr);
		}
	}

	// Luna: Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = arrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = arrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = textureCount;

	HR(
		device->CreateShaderResourceView(textureArray.Get(), &viewDesc, textureViewArray));

	// Cleanup - we only need the resource view.
	textureArray.Reset();
	for (UINT i = 0; i < textureCount; ++i)
		textures[i].Reset();
}

ID3D11ShaderResourceView* d3dHelper::CreateTexture2DArraySRV(
	ID3D11Device* device, ID3D11DeviceContext* context,
	std::vector<std::wstring>& filenames,
	DXGI_FORMAT format,
	UINT filter,
	UINT mipFilter)
{
	//
	// Load the texture elements individually from file.  These textures
	// won't be used by the GPU (0 bind flags), they are just used to 
	// load the image data from file.  We use the STAGING usage so the
	// CPU can read the resource.
	//

	UINT size = filenames.size();

	std::vector<ID3D11Texture2D*> srcTex(size);
	for (UINT i = 0; i < size; ++i)
	{
	/*	D3DX11_IMAGE_LOAD_INFO loadInfo;

		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.Depth = D3DX11_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX11_FROM_FILE;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = format;
		loadInfo.Filter = filter;
		loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo = 0;

		HR(D3DX11CreateTextureFromFile(device, filenames[i].c_str(),
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0));*/

		const wchar_t* szName = filenames[i].c_str();

		//ID3D11Resource* texResource = nullptr;
		//HR(DirectX::CreateDDSTextureFromFile(device, szName,0, D3D11_USAGE_STAGING,0, 0,0,false, (ID3D11ShaderResourceView*)srcTex[i]));
		//ReleaseCOM(texResource); // view saves reference

	/*	CreateDDSTextureFromFileEx(
			_In_ ID3D11Device * d3dDevice,
			_In_z_ const wchar_t* szFileName,
			_In_ size_t maxsize,
			_In_ D3D11_USAGE usage,
			_In_ unsigned int bindFlags,
			_In_ unsigned int cpuAccessFlags,
			_In_ unsigned int miscFlags,
			_In_ bool forceSRGB,
			_Outptr_opt_ ID3D11Resource * *texture,
			_Outptr_opt_ ID3D11ShaderResourceView * *textureView,
			_Out_opt_ DDS_ALPHA_MODE * alphaMode = nullptr) noexcept;*/
	}

	//
	// Create the texture array.  Each element in the texture 
	// array has the same format/dimensions.
	//

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	HR(device->CreateTexture2D(&texArrayDesc, 0, &texArray));

	//
	// Copy individual texture elements into texture array.
	//

	// for each texture element...
	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		// for each mipmap level...
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(context->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			context->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(srcTex[texElement], mipLevel);
		}
	}

	//
	// Create a resource view to the texture array.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	ID3D11ShaderResourceView* texArraySRV = 0;
	HR(device->CreateShaderResourceView(texArray, &viewDesc, &texArraySRV));

	//
	// Cleanup--we only need the resource view.
	//

	ReleaseCOM(texArray);

	for (UINT i = 0; i < size; ++i)
		ReleaseCOM(srcTex[i]);

	return texArraySRV;
}


ID3D11ShaderResourceView* d3dHelper::CreateRandomTexture1DSRV(ID3D11Device* device)
{
	// 
	// Create the random data.
	//
	XMFLOAT4 randomValues[1024];

	for(int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);
	}

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024*sizeof(XMFLOAT4);
    initData.SysMemSlicePitch = 0;

	//
	// Create the texture.
	//
    D3D11_TEXTURE1D_DESC texDesc;
    texDesc.Width = 1024;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
    HR(device->CreateTexture1D(&texDesc, &initData, &randomTex));

	//
	// Create the resource view.
	//
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
    viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;
	
	ID3D11ShaderResourceView* randomTexSRV = 0;
    HR(device->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV));

	ReleaseCOM(randomTex);

	return randomTexSRV;
}

void ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX T)
{
	XMFLOAT4X4 M;
	XMStoreFloat4x4(&M, T);

	//
	// Left
	//

	
	planes[0].x = M(0,3) + M(0,0);
	planes[0].y = M(1,3) + M(1,0);
	planes[0].z = M(2,3) + M(2,0);
	planes[0].w = M(3,3) + M(3,0);

	//
	// Right
	//
	planes[1].x = M(0,3) - M(0,0);
	planes[1].y = M(1,3) - M(1,0);
	planes[1].z = M(2,3) - M(2,0);
	planes[1].w = M(3,3) - M(3,0);

	//
	// Bottom
	//
	planes[2].x = M(0,3) + M(0,1);
	planes[2].y = M(1,3) + M(1,1);
	planes[2].z = M(2,3) + M(2,1);
	planes[2].w = M(3,3) + M(3,1);

	//
	// Top
	//
	planes[3].x = M(0,3) - M(0,1);
	planes[3].y = M(1,3) - M(1,1);
	planes[3].z = M(2,3) - M(2,1);
	planes[3].w = M(3,3) - M(3,1);

	//
	// Near
	//
	planes[4].x = M(0,2);
	planes[4].y = M(1,2);
	planes[4].z = M(2,2);
	planes[4].w = M(3,2);

	//
	// Far
	//
	planes[5].x = M(0,3) - M(0,2);
	planes[5].y = M(1,3) - M(1,2);
	planes[5].z = M(2,3) - M(2,2);
	planes[5].w = M(3,3) - M(3,2);

	// Normalize the plane equations.
	for(int i = 0; i < 6; ++i)
	{
		XMVECTOR v = XMPlaneNormalize(XMLoadFloat4(&planes[i]));
		XMStoreFloat4(&planes[i], v);
	}
}