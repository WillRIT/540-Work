
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
    float4 colorTint;   // RGBA color tint - 16 bytes
    float time;         // Time value for animations - 4 bytes
    float3 padding;     // Padding to make buffer 32 bytes (multiple of 16)
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
    // Cool animated shader effect using UV coordinates and normals
    
    // Animated waves based on UV and time
    float wave1 = sin(input.uv.x * 10.0 + time * 2.0) * 0.5 + 0.5;
    float wave2 = cos(input.uv.y * 10.0 + time * 1.5) * 0.5 + 0.5;
    
    // Create animated color based on normals
    float3 animatedNormal = input.normal * 0.5 + 0.5; // Remap from [-1,1] to [0,1]
    
    // Pulsing effect
    float pulse = sin(time * 3.0) * 0.3 + 0.7;
    
    // Combine waves with normals for interesting color variation
    float3 baseColor = float3(
        wave1 * animatedNormal.r,
        wave2 * animatedNormal.g,
        (wave1 + wave2) * 0.5 * animatedNormal.b
    ) * pulse;
    
    // Apply color tint
    float3 finalColor = baseColor * colorTint.rgb;
    
    // Add some fresnel-like rim lighting effect
    float3 viewDir = normalize(float3(0, 0, 1)); // Simple approximation
    float rimIntensity = pow(1.0 - saturate(dot(input.normal, viewDir)), 3.0);
    finalColor += rimIntensity * float3(1, 1, 1) * 0.3;
    
    return float4(finalColor, colorTint.a);
}
