#include "ShaderInclude.hlsli"
#include "Lighting.hlsli"

//data to manipulate pixels
cbuffer ShaderData : register(b0)
{
    int numLights;
    float3 ambientColor;

    Lights lights[MAX_LIGHTS];

    float3 cameraPos;
    float pad;

    float4 colorTint;
    float roughness;
    float3 padding;
    float2 uvScale;
    float2 uvOffset;
};

Texture2D Albedo : register(t0); //whiteness map (surface texture)
Texture2D NormalMap : register(t1); //normal map
Texture2D RoughnessMap : register(t2); //roughness map (similar to specular maps)
Texture2D MetalnessMap : register(t3); //metalness map (0 or 1)
Texture2D ShadowMap : register(t4);
SamplerState BasicSampler : register(s0); //"s" registers for samplers

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
	//check shadow map before lighting
    input.shadowMapPos /= input.shadowMapPos.w;
	//convert the normalized device coordinates to UVs for sampling
    float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
    shadowUV.y = 1 - shadowUV.y; // Flip the Y
	//grab the distances we need: light-to-pixel and closest-surface
    float distToLight = input.shadowMapPos.z;
    float distShadowMap = ShadowMap.Sample(BasicSampler, shadowUV).r;
	//for testing, just return black where there are shadows.
    if (distShadowMap < distToLight)
        return float4(0, 0, 0, 1);
	
	
	//alter uv coords using offset
    input.uv = input.uv * uvScale + uvOffset;

	//normalize input normal
    input.normal = normalize(input.normal);
	//normalize input tangent
    input.tangent = normalize(input.tangent);
	
    input.normal = NormalMapping(NormalMap, BasicSampler, input.uv, input.normal, input.tangent);
	

	//"sample" the texture and color
	//this gives output color
	//use pow for "unpacking" gamma correction
    float3 color = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f); //swizzle using logical indices
	
	//detemrined by a single float, simply sample the red channel
    float roughnessFromMap = RoughnessMap.Sample(BasicSampler, input.uv).r;
	
	//also a single float
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
	
	//now, a specular color is needed
	//metals generally tint reflections, while nonmetals generally have
	//the same specular color throughout (obtained using 0.04)
	//metalness is usually 0 or 1, bu might be inbetween because of
	//linear texture sampling, lerp is used on the specular color to match this
    float3 specularColor = lerp(0.04f, color.rgb, metalness);
	
	//Begin lighting calculations
	//include ambient lighting ONCE
    float3 totalLight = float3(0, 0, 0);
	
	//angle the surface is viewed from
    float3 surfaceToCamera = normalize(cameraPos - input.worldPosition);
	
	//apply the total lighting
    totalLight += CalculateTotalLightPBR(numLights, lights, input.normal, surfaceToCamera, input.worldPosition, roughnessFromMap, metalness, color, specularColor);
	
	
	 // Gamma Correction
    totalLight = pow(totalLight, 1.0 / 2.2);
	
	
	//return modified color
    return float4(totalLight, 1);
}