#include "Test.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    float32_t2 uv = input.texcoord;
    PixelShaderOutput output;
    output.color = float32_t4(uv.x, uv.y, 0.0f, 0.0f);
    return output;
    
}
