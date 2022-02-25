#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <DirectXMath.h>
#include "LightHelper.h"
#include "Vertex.h"
#include "Effects.h"

using namespace DirectX;

class AssimpMesh {
public:
    std::vector<Vertex::PosNormalTexTanSkinned> vertices;
    std::vector<UINT> indices;
    std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
    std::vector<ID3D11ShaderResourceView*> NormalMapSRV;
    std::vector <Material> Mat;
    ID3D11Device* device;

    AssimpMesh()
    {
    }

    AssimpMesh(ID3D11Device* dev, const std::vector<Vertex::PosNormalTexTanSkinned>& vertices, const std::vector<UINT>& indices, const std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV, const std::vector<ID3D11ShaderResourceView*> NormalMapSRV, std::vector <Material> Mat) :
        vertices(vertices),
        indices(indices),
        DiffuseMapSRV(DiffuseMapSRV),
        NormalMapSRV(NormalMapSRV),
        Mat(Mat),
        device(dev),
        VertexBuffer(nullptr),
        IndexBuffer(nullptr) {
        this->setupMesh(device);
    }

    void Draw(ID3D11DeviceContext* devcon, ID3DX11EffectTechnique* tech, bool tex) {
        UINT stride = sizeof(Vertex::PosNormalTexTanSkinned);
        UINT offset = 0;

        devcon->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
        devcon->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        if (tex = true)
        {
            //Effects::NormalMapFX->SetDiffuseMap(DiffuseMapSRV[0]);
            //devcon->PSSetShaderResources(0, 1, &DiffuseMapSRV[0]);
            tech->GetPassByIndex(0)->Apply(0, devcon);
        }
        devcon->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
    }

private:
    // Render data
    ID3D11Buffer* VertexBuffer, *IndexBuffer;

    // Functions
    // Initializes all the buffer objects/arrays
    void setupMesh(ID3D11Device* dev) {
        HRESULT hr;

        D3D11_BUFFER_DESC vbd;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex::PosNormalTexTanSkinned) * vertices.size());
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = 0;
        vbd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &vertices[0];

        hr = dev->CreateBuffer(&vbd, &initData, &VertexBuffer);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create vertex buffer.");
        }

        D3D11_BUFFER_DESC ibd;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = 0;
        ibd.MiscFlags = 0;

        initData.pSysMem = &indices[0];

        hr = dev->CreateBuffer(&ibd, &initData, &IndexBuffer);
        if (FAILED(hr)) {
            throw std::runtime_error("Failed to create index buffer.");
        }
    }
};

