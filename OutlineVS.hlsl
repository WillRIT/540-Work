#include "ShaderInclude.hlsli"

#include "ShaderInclude.hlsli"

cbuffer OutlineVertexConstants : register(b0)
{
    matrix world;
    matrix worldInvTranspose;
    matrix view;
    matrix projection;
    float outlineThickness;
    float3 padding;
};

struct OutlineVertexToPixel
{
    float4 screenPosition : SV_POSITION;
};

OutlineVertexToPixel main(VertexShaderInput input)
{
    OutlineVertexToPixel output;

    float4 worldPos = mul(world, float4(input.localPosition, 1.0f));
    float3 normalWs = normalize(mul((float3x3)worldInvTranspose, input.normal));

    float4 viewPos = mul(view, worldPos);
    float3 normalVs = normalize(mul((float3x3)view, normalWs));
    viewPos.xyz += normalVs * outlineThickness;

    output.screenPosition = mul(projection, viewPos);

    return output;
}
