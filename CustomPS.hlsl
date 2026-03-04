
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

cbuffer PixelShaderConstants : register(b0)
{
    float4 colorTint; // RGBA color (red, green, blue, alpha)
    float deltaTime; // Time since last frame
    float2 resolution; // Screen width
};

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
		//inspired by a shader made by The_King_Of_Shader
	//https://www.shadertoy.com/view/WcXSR2
    
    //speed up shader
    float time = deltaTime * 4;
    
    //create coordinates based on color of pixel, and screen resolution
    float2 uv = (input.screenPosition.xy - float2(0.5f, 0.5f) * resolution.xy) / resolution.xx;
    float2 uv2 = (input.screenPosition.xy - float2(0.5f, 0.505f) * resolution.xy) / resolution.xx;
    float2 uv3 = (input.screenPosition.xy - float2(0.505f, 0.5f) * resolution.xy) / resolution.xx;
    
    //generate circles stemming from uv positions
    float circle = (length(uv) + (5 * (time - 0.02))) * 40;
    float circle2 = (length(uv2) + (5 * (time - 0.02))) * 40;
    float circle3 = (length(uv3) + (5 * (time - 0.02))) * 40;

    //get change in circles over time
    float changeCircle = (sin(circle) + 1) / 2;
    float changeCircle2 = (sin(circle2) + 1) / 2;
    float changeCircle3 = (sin(circle3) + 1) / 2;
    
    //subtract changes from one another and
    //multiply the changes by an intensity
    float cy = (changeCircle - changeCircle2) * 10;
    float cx = (changeCircle - changeCircle3) * 10;

    //compute resulting color
    float3 result = normalize(cross(float3(1, 0, cy), float3(0, 1, cx)));
    //apply color tint
    result *= float3(1, 0, 0);
    //return result
    return float4(result, 1);
	
}