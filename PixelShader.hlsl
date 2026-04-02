// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
#include "ShaderInclude.hlsli"

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers

cbuffer PixelShaderConstants : register(b0)
{
    // lighting
    int lights;
    float3 ambientColor;
    
    // camera stuff
    float3 cameraPosition;
    float pad; 
    
 
    float4 colorTint; // RGBA color (red, green, blue, alpha)
    float time;
    float3 padding;
    float2 uvScale;
    float2 uvOffset;
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
    input.normal = normalize(input.normal);
    input.uv = input.uv * uvScale + uvOffset;

    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv);
    surfaceColor = ambientColor * surfaceColor * colorTint.rgb;
        
    return float4(surfaceColor, 1);
}