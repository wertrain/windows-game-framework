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
    column_major float4x4 Matrix : MATRIX;
    uint InstanceId : SV_InstanceID;
};

struct VS_OUT
{
    float4 Pos : SV_POSITION;
    float3 UV : TEXCOORD0;
};

PS_INPUT vsMain(VS_INPUT vsInput)
{
    VS_OUT vsOut;
    Out.Pos = mul(float4(vsInput.Pos, 1.0f), vsInput.Matrix);
    Out.UV = float3(vsInput.UV, vsInput.InstanceId % 3);
    return Out;
}
