 // Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
#include "ShaderInclude.hlsli"

cbuffer VertexShaderConstants : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
    matrix worldInvTranspose;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	// Transform the vertex position through world, view, and projection matrices
	// The W component is set to 1.0 for proper matrix multiplication
	// - Each component is automatically divided by W in the rasterizer for perspective divide
    matrix wvp = mul(projection, mul(view, world));
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
	
	// Transform the normal to world space (for lighting calculations)
	// We only care about rotation, not translation, so we cast to float3x3
    output.uv = input.uv;

	output.normal = mul((float3x3)worldInvTranspose, input.normal);
    output.worldPosition = mul(world, float4(input.localPosition, 1.0f)).xyz;
	
	// Pass through UV coordinates unchanged

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}