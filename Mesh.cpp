#include "Mesh.h"
#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include <vector>

#include <DirectXMath.h>
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "Entity.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;


Mesh::Mesh(Vertex vertices[], unsigned int indices[], int verts, int inds)
{
	MakeBuffers(vertices, verts, indices, inds);
	
}
Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return indCount;
}

int Mesh::GetVertexCount()
{
	return vertCount;
}

void Mesh::MakeBuffers(Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices)
{
	// Create the vertex buffer
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * (UINT)vertCount; // Number of vertices
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = vertArray;
	Graphics::Device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());

	// Create the index buffer
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * (UINT)indCount; // Number of indices
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	
	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	initialIndexData.pSysMem = indexArray;
	
	Graphics::Device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());

	// Save the counts
	this->indCount = (unsigned int)numIndices;
	this->vertCount = (unsigned int)numVerts;
}

void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	Graphics::Context->DrawIndexed(
		GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}

Mesh::~Mesh()
{
}