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
    int4 boneIndex : BONEINDEX;
    float4 boneWeight : BONEWEIGHT;
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

struct MATERIAL
{
    float4 Ambient; // 環境反射
    float4 Diffuse; // 拡散反射
    float4 Specular; // 鏡面反射
    float4 Emission; // 発光
    float Shiness; // 光沢のなめらかさ
    bool TextureEnable; // テクスチャを使うか否か
    bool2 Dummy;
};

cbuffer MaterialBuffer : register(b4)
{
    MATERIAL Material;
}

// UV座標移動行列
cbuffer TextureBuffer : register(b5)
{
    matrix matrixTex;
}

cbuffer BoneMatrixBuffer : register(b6)
{
    matrix BoneMatrix[400];
}