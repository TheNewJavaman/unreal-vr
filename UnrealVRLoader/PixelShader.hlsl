cbuffer C_BUFFER
{
    float2 Offsets;
};

SamplerState s0;
Texture2D tex0;

struct PS_INPUT
{
    float4 Position : SV_Position;
    float2 TexCoords : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target0
{
    return tex0.Sample(s0, input.TexCoords + Offsets);
}