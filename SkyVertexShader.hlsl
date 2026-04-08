

struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VertexToPixel
{
    float4 position : SV_Position;
    float3 sampleDir : DIRECTION;
};


cbuffer SkyVertexShaderConstants : register(b0)
{
	matrix view;
	matrix projection;
};


VertexToPixel main( VertexShaderInput input )
{
    //Set up output struct
    VertexToPixel output;
    
    
    matrix viewCopy = view;
    
    viewCopy._14 = 0;
    viewCopy._24 = 0;
    viewCopy._34 = 0;
    
    output.position = mul(projection, mul(viewCopy, float4(input.localPosition, 1.0f)));
    
    output.position.z = output.position.w; // Set depth to max (far plane)
    output.sampleDir = input.localPosition; // Sample direction is just the vertex position
    
    return output;
    
}