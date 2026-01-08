#pragma once
#include	<wrl/client.h>
#include	<string>
#include	<d3d11.h>

using Microsoft::WRL::ComPtr;

//=======================================
//Shaderクラス
//=======================================
class Shader{
private:
	int m_rastrizerState = 0;
public:
	//=======================================
	// コンストラクタ・デストラクタ
	//=======================================
	Shader() {}
	~Shader() {}
	//=======================================
	// その他関数
	//=======================================
	void Create(std::string vs, std::string ps);
	void SetGPU();
	void SetRasterizeState(int state);
private:
	ComPtr<ID3D11VertexShader> m_pVertexShader;		// 頂点シェーダー
	ComPtr<ID3D11PixelShader>  m_pPixelShader;		// ピクセルシェーダー
	ComPtr<ID3D11InputLayout>  m_pVertexLayout;		// 頂点レイアウト
};

