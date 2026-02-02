#pragma once

#include <d3d11.h>
#include <wrl/client.h>

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
	 
	void GetVertexBuffer();
	void GetIndexBuffer();
	int GetIndexCount();
	int GetVertexCount();
	void Draw();

	~Mesh();
	Mesh(int vertexCount, int indexCount, const Vertex* verts, const unsigned int* indices);

};

