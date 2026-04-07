#include "Mesh.h"
#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"
#include <vector>
#include <iostream>
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

/// <summary>
/// Construct a mesh from a file. Uses tinyobjloader to load .obj files
/// </summary>
/// <param name="fileName"></param>
Mesh::Mesh(const char* fileName)
{
	tinyobj::ObjReaderConfig readerConfig;

	readerConfig.mtl_search_path = "Assets/Meshes/"; // Path to material files

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(fileName, readerConfig)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}
		exit(1);
	}

	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	auto& materials = reader.GetMaterials();

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Loop over shapes (usually just one shape per file)
	for (const auto& shape : shapes) {
		size_t index_offset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
			size_t fv = shape.mesh.num_face_vertices[f];
			
			// Loop over vertices in the face (usually 3 for triangles)
			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
				
				Vertex vertex = {};
				
				// Get position data
				vertex.Position.x = attrib.vertices[3 * idx.vertex_index + 0];
				vertex.Position.y = attrib.vertices[3 * idx.vertex_index + 1];
				vertex.Position.z = attrib.vertices[3 * idx.vertex_index + 2];
				
				// Get UV data (if available)
				if (idx.texcoord_index >= 0) {
					vertex.UV.x = attrib.texcoords[2 * idx.texcoord_index + 0];
					vertex.UV.y = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1]; // Flip V coordinate
				}
				
				// Get normal data (if available)
				if (idx.normal_index >= 0) {
					vertex.Normal.x = attrib.normals[3 * idx.normal_index + 0];
					vertex.Normal.y = attrib.normals[3 * idx.normal_index + 1];
					vertex.Normal.z = attrib.normals[3 * idx.normal_index + 2];
				}
				
				// Add vertex and index
				vertices.push_back(vertex);
				indices.push_back((unsigned int)vertices.size() - 1);
			}
			index_offset += fv;
		}
	}
	
	// Calculate tangents for normal mapping
	CalculateTangents(vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size());
	// Now call MakeBuffers with the populated vectors
	MakeBuffers(vertices.data(), vertices.size(), indices.data(), indices.size());
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
// Author: Chris Cascioli
// Purpose: Calculates the tangents of the vertices in a mesh
// 
// - You are allowed to directly copy/paste this into your code base
//   for assignments, given that you clearly cite that this is not
//   code of your own design.
//
// - Code originally adapted from: http://www.terathon.com/code/tangent.html
//   - Updated version now found here: http://foundationsofgameenginedev.com/FGED2-sample.pdf
//   - See listing 7.4 in section 7.5 (page 9 of the PDF)
//
// - Note: For this code to work, your Vertex format must
//         contain an XMFLOAT3 called Tangent
//
// - Be sure to call this BEFORE creating your D3D vertex/index buffers
void Mesh::CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices)
{
	// Reset tangents
	for (int i = 0; i < numVerts; i++)
	{
		verts[i].Tangent = XMFLOAT3(0, 0, 0);
	}

	// Calculate tangents one whole triangle at a time
	for (int i = 0; i < numIndices;)
	{
		// Grab indices and vertices of first triangle
		unsigned int i1 = indices[i++];
		unsigned int i2 = indices[i++];
		unsigned int i3 = indices[i++];
		Vertex* v1 = &verts[i1];
		Vertex* v2 = &verts[i2];
		Vertex* v3 = &verts[i3];

		// Calculate vectors relative to triangle positions
		float x1 = v2->Position.x - v1->Position.x;
		float y1 = v2->Position.y - v1->Position.y;
		float z1 = v2->Position.z - v1->Position.z;

		float x2 = v3->Position.x - v1->Position.x;
		float y2 = v3->Position.y - v1->Position.y;
		float z2 = v3->Position.z - v1->Position.z;

		// Do the same for vectors relative to triangle uv's
		float s1 = v2->UV.x - v1->UV.x;
		float t1 = v2->UV.y - v1->UV.y;

		float s2 = v3->UV.x - v1->UV.x;
		float t2 = v3->UV.y - v1->UV.y;

		// Create vectors for tangent calculation
		float r = 1.0f / (s1 * t2 - s2 * t1);

		float tx = (t2 * x1 - t1 * x2) * r;
		float ty = (t2 * y1 - t1 * y2) * r;
		float tz = (t2 * z1 - t1 * z2) * r;

		// Adjust tangents of each vert of the triangle
		v1->Tangent.x += tx;
		v1->Tangent.y += ty;
		v1->Tangent.z += tz;

		v2->Tangent.x += tx;
		v2->Tangent.y += ty;
		v2->Tangent.z += tz;

		v3->Tangent.x += tx;
		v3->Tangent.y += ty;
		v3->Tangent.z += tz;
	}

	// Ensure all of the tangents are orthogonal to the normals
	for (int i = 0; i < numVerts; i++)
	{
		// Grab the two vectors
		XMVECTOR normal = XMLoadFloat3(&verts[i].Normal);
		XMVECTOR tangent = XMLoadFloat3(&verts[i].Tangent);

		// Use Gram-Schmidt orthonormalize to ensure
		// the normal and tangent are exactly 90 degrees apart
		tangent = XMVector3Normalize(
			tangent - normal * XMVector3Dot(normal, tangent));

		// Store the tangent
		XMStoreFloat3(&verts[i].Tangent, tangent);
	}
}
void Mesh::MakeBuffers(Vertex* vertArray, size_t numVerts, unsigned int* indexArray, size_t numIndices)
{
	this->indCount = (unsigned int)numIndices;
	this->vertCount = (unsigned int)numVerts;

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

}

void Mesh::Draw()
{
		// DRAW geometry
		// - These steps are generally repeated for EACH object you draw
		// - Other Direct3D calls will also be necessary to do more complex things

		// Set buffers in the input assembler (IA) stage
		//  - Do this ONCE PER OBJECT, since each object may have different geometry
		//  - For this demo, this step *could* simply be done once during Init()
		//  - However, this needs to be done between EACH DrawIndexed() call
		//     when drawing different geometry, so it's here as an example
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Graphics::Context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
		Graphics::Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);



		// Tell Direct3D to draw
		//  - Begins the rendering pipeline on the GPU
		//  - Do this ONCE PER OBJECT you intend to draw
		//  - This will use all currently set Direct3D resources (shaders, buffers, etc)
		//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
		//     vertices in the currently set VERTEX BUFFER
		Graphics::Context->DrawIndexed(
			indCount,    // The number of indices to use (we could draw a subset if we wanted)
			0,			// Offset to the first index we want to use
			0);			// Offset to add to each index when looking up vertices
}

Mesh::~Mesh()
{
}