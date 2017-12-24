//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

cbuffer ConstBuff : register(b0)
{
    float2 mtxWorld;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float4 UV : TEXCOORD;
    column_major float4x4 Matrix : MATRIX;
    uint InstanceId : SV_InstanceID; // インスタンシング描画 ID （要 Shader Model 4.0 以上）
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
    output.Pos = mul(input.Pos, input.Matrix);
    output.Color = input.Color;
    output.UV = float4(input.UV.x, input.UV.y, input.InstanceId % 3, 0.0);
    return output;
}
