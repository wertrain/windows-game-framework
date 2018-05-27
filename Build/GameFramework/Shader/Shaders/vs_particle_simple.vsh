//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

cbuffer ConstBuff : register(b0)
{
    matrix mtxWorld;
};

//--------------------------------------------------------------------------------------

struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float4 UV : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float4 UV : TEXCOORD;
};

PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, mtxWorld);
    output.Color = input.Color;
    output.UV = input.UV;
    return output;
}
