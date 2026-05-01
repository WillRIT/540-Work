// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
#include "ShaderInclude.hlsli"
#include "Lighting.hlsli"

// PBR Texture Slots
Texture2D Albedo : register(t0);
Texture2D NormalMap : register(t1);
Texture2D ShadowMap : register(t4);
SamplerState BasicSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);


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
 input.shadowMapPos /= input.shadowMapPos.w;
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y;

    float distToLight = input.shadowMapPos.z;
    float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, distToLight).r;

    
    
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.uv = input.uv * uvScale + uvOffset;

    
    input.normal = NormalMapping(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);
    
    float3 surfaceColor = Albedo.Sample(BasicSampler, input.uv).rgb;

    // also gamma correct this
    surfaceColor = pow(surfaceColor, 2.2);
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

        float3 lightResult = float3(0, 0, 0);
        switch (currentLight.type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                lightResult = DirectionLight(currentLight, input.normal, surfaceToCamera, roughness, surfaceColor);
                if (i == 0)
                {
                    lightResult *= shadowAmount;
                }
                break;
            case LIGHT_TYPE_POINT:
                lightResult = PointLight(currentLight, input.normal, surfaceToCamera, input.worldPosition, roughness, surfaceColor);
                break;
            case LIGHT_TYPE_SPOT:
                lightResult = SpotLight(currentLight, input.normal, surfaceToCamera, input.worldPosition, roughness, surfaceColor);
                break;
        }

        totalLight += lightResult;
    }
    
    // Gamma Correction
    totalLight = pow(totalLight, 1.0 / 2.2);
    
    return float4(totalLight, 1);
}