#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"


class Mesh
{
private:
	 Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	 Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	 
	 // this might not be the best way to name these but 
	 // this is the Vertex Buffers Vertices and the IndexBuffers IndicesGe respectively
	 int vertCount; // used when drawing
	 int indCount; // good for the UI


public:
	 
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	void Draw();

	~Mesh();
	Mesh(Vertex vertices[], unsigned int indices[], int verts, int inds);

};

