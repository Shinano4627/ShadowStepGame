cbuffer WorldBuffer : register(b0)
{
	matrix World;
}
cbuffer ViewBuffer : register(b1)
{
	matrix View;
}
cbuffer ProjectionBuffer : register(b2)
{
	matrix Projection;
}

struct VS_IN
{
    float4 pos : POSITION0;
	float4 nrm : NORMAL0;
    float4 col : COLOR0;
    float2 tex : TEXCOORD0;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

struct LIGHT
{
    bool Enable;    // 使用するかどうか
    bool3 Dummy; // PADDING
    float4 Direction; // 方向
    float4 Diffuse; // 平行光源の強さと色
    float4 Ambient; // 環境光の強さと色
};

cbuffer LightBuffer : register(b3)
{
    LIGHT Light;
}
