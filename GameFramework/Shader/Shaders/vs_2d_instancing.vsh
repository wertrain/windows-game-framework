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
    output.Pos = input.Pos + input.InstancePos;
    //output.Pos = mul(input.InstancePos, mtxWorld);
    //output.Pos = mul(output.Pos, mtxView);
    //output.Pos = mul(output.Pos, mtxProj);
    //output.Pos = mul(output.Pos, input.Matrix);
    output.Color = input.Color;
    //output.UV = float4(input.UV.x, input.UV.y, input.InstanceId % 3, 0.0);
    return output;
}
