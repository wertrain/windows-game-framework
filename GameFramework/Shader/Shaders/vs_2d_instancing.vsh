//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float4 UV : TEXCOORD;
    float4 InstancePos : INSTANCEPOS;
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
    output.Pos = input.Pos + float4(input.InstancePos.x, input.InstancePos.y, 0.0f, 0.0f);
    output.Color = input.Color;
    output.UV = float4(input.UV.x + input.InstancePos.z, input.UV.y + input.InstancePos.w, 0.0f, 0.0f);
    //output.UV = float4(input.UV.x, input.UV.y, 0.0f, 0.0f);
    return output;
}
