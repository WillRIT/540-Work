#include "ShaderInclude.hlsli"

cbuffer OutlinePixelConstants : register(b0)
{
    float4 outlineColor;
};

float4 main() : SV_TARGET
{
    return outlineColor;
}
