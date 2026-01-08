#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include "Renderer.h"

//--------------------
// UnitModel
// ・ユニットの描画専用クラス
// ・頂点/シェーダー/描画処理を管理
//--------------------

class UnitModel
{
public:
	UnitModel();
	~UnitModel();

	//初期化
	bool Init();

	//描画
	//world:Unitが持つワールド行列
	void Draw(const DirectX::SimpleMath::Matrix& world);

	//解放
	void Uninit();

private:
	//バッファ
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	//シェーダー
	ID3D11VertexShader* m_pVertexShader = nullptr;
	ID3D11PixelShader* m_pPixelShader = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;

	UINT m_IndexCount = 0;
};