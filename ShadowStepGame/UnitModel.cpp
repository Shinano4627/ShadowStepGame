#include "UnitModel.h"

using namespace DirectX::SimpleMath;

struct VERTEX
{
	Vector3 pos;
	Color	color;
};

UnitModel::UnitModel()
{

}

UnitModel::~UnitModel()
{

}

bool UnitModel::Init()
{
	//--------------------
	//頂点データ(とりあえず三角形)
	//--------------------
	VERTEX vertices[] =
	{
		{{0.0f,1.0f,0.0f},Color(1,0,0,1)},
		{{1.0f,-1.0f,0.0f},Color(0,1,0,1)},
		{{-1.0f,-1.0f,0.0f},Color(0,0,1,1)},
	};

	unsigned int indices[] = { 0,1,2 };
	m_IndexCount = 3;

	//頂点バッファ
	Renderer::CreateVertexBuffer(
		sizeof(VERTEX),
		3,
		vertices,
		&m_pVertexBuffer
	);

	//インデックスバッファ
	Renderer::CreateIndexBuffer(
		3,
		indices,
		&m_pIndexBuffer
	);

	//シェーダー
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		 { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,
		  D3D11_INPUT_PER_VERTEX_DATA,0 },

		{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,
		  D3D11_INPUT_PER_VERTEX_DATA,0 },
	};

	Renderer::CreateVertexShader(
		&m_pVertexShader,
		&m_pInputLayout,
		layout,
		2,
		"VertexShader.hlsl"
	);

	Renderer::CreatePixelShader(
		&m_pPixelShader,
		"PixelShader.hlsl"
	);

	return true;
}

void UnitModel::Draw(const Matrix& world)
{
	auto context = Renderer::GetDeviceContext();

	//ワールド行列設定
	Renderer::SetWorldMatrix(const_cast<Matrix*>(&world));

	//頂点バッファ設定
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//シェーダー設定
	context->IASetInputLayout(m_pInputLayout);
	context->VSSetShader(m_pVertexShader, nullptr, 0);
	context->PSSetShader(m_pPixelShader, nullptr, 0);

	//描画
	context->DrawIndexed(m_IndexCount, 0, 0);
}

void UnitModel::Uninit()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pInputLayout);
}