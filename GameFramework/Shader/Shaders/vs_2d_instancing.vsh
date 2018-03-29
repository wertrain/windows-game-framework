//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 UV : TEXCOORD;
    float4 InstancePos : IPOSITION;
    uint InstanceId : SV_InstanceID; // インスタンシング描画 ID （要 Shader Model 4.0 以上）
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

PS_INPUT vsMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = float4(input.Pos, 1.0f) + float4(input.InstancePos.x, input.InstancePos.y, 0.0f, 0.0f);
    output.UV = input.UV + input.InstancePos.zw;
    return output;
}
