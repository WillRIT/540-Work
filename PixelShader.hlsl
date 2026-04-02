// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
#include "ShaderInclude.hlsli"
#include "Lighting.hlsli"

Texture2D SurfaceTexture : register(t0); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers

cbuffer PixelShaderConstants : register(b0)
{
    // lighting - MUST match C++ struct order exactly!
    int numLights;
    float3 ambientColor;
    
    Lights lights[MAX_LIGHTS];
    
    // camera stuff
    float3 cameraPosition;
    float pad;
    
    float4 colorTint;
    float roughness;
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

    float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
    surfaceColor *= colorTint.rgb;
    
    float3 totalLight = (surfaceColor * ambientColor);
        
    float3 surfaceToCamera = normalize(cameraPosition - input.worldPosition);

     //loop through lights
    for (int i = 0; i < numLights; i++)
    {
        //grab a copy of the current index
        Lights currentLight = lights[i];
        //normalize the direction to ensure consistent results
        currentLight.direction = normalize(currentLight.direction);
        
        switch (currentLight.type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionLight(currentLight, input.normal, surfaceToCamera, roughness, surfaceColor);
                break;
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(currentLight, input.normal, surfaceToCamera, input.worldPosition, roughness, surfaceColor);
                break;
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(currentLight, input.normal, surfaceToCamera, input.worldPosition, roughness, surfaceColor);
                break;
        }
    }
    
    
    return float4(totalLight, 1);
}