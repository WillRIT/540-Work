
struct VertexToPixel
{
    float4 position : SV_Position;
    float3 sampleDir : DIRECTION;
};

SamplerState BasicSampler : register(s0); // "s" registers for samplers
TextureCube SurfaceTexture : register(t0); // "t" registers for textures


float4 main(VertexToPixel input) : SV_TARGET
{
	    // Sample the cubemap using the sample direction
    float3 color = SurfaceTexture.Sample(BasicSampler, input.sampleDir).rgb;
    
    return float4(color, 1.0f); // Return the sampled color with full opacity
}