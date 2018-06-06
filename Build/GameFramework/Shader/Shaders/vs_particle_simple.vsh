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

    float center_x = input.Pos.z; // z 座標にはサイズの半分の値が入っている
    float center_y = input.Pos.z;
    // 中心点で回転したい
    float4 center_pos = float4(input.Pos.x - center_x, input.Pos.y - center_y, 1.0f, 1.0f);

    // 回転行列の作成
    float rot = input.InstancePos.z;
    matrix rotMatrix = matrix
    (
        cos(rot), sin(rot), 0.0, 0.0,
        -sin(rot), cos(rot), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
    float4 v_pos = mul(center_pos, rotMatrix);

    float4 pos = float4(input.InstancePos.x, input.InstancePos.y, 0.0f, 0.0f);
    output.Pos = v_pos + pos;
        
    output.UV = input.UV;
    return output;
}
