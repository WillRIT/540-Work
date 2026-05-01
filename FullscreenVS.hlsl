struct VertexToPixel
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexToPixel main(uint id : SV_VertexID)
{
    VertexToPixel output;

    if (id == 0)
    {
        output.position = float4(-1, 1, 0, 1);
        output.uv = float2(0, 0);
    }
    else if (id == 1)
    {
        output.position = float4(3, 1, 0, 1);
        output.uv = float2(2, 0);
    }
    else
    {
        output.position = float4(-1, -3, 0, 1);
        output.uv = float2(0, 2);
    }

    return output;
}
