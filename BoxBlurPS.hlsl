
cbuffer ExternalData : register(b0)
{
    int blurRadius;
    float pixelWidth;
    float pixelHeight;
    float padding;
};

static const int MAX_BLUR_RADIUS = 4;

struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D Pixels : register(t0);
SamplerState ClampSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 total = 0;
    int sampleCount = 0;

    for (int x = -MAX_BLUR_RADIUS; x <= MAX_BLUR_RADIUS; x++)
    {
        for (int y = -MAX_BLUR_RADIUS; y <= MAX_BLUR_RADIUS; y++)
        {
            if (abs(x) <= blurRadius && abs(y) <= blurRadius)
            {
                float2 uv = input.uv;
                uv += float2(x * pixelWidth, y * pixelHeight);
                total += Pixels.Sample(ClampSampler, uv);
                sampleCount++;
            }
        }
    }

    return total / sampleCount;
}
