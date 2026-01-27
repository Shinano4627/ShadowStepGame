// ===================================================================
// SystemCommon.h 全プラットフォーム共通ヘッダー
// デバッグ用途以外で、特定のプラットフォームでのみ使用可能なものは記載しないこと。
// ===================================================================
#pragma once
#define DEBUG

#include <string>
#include <SimpleMath.h>		// Vector構造体等はSimpleMathを使用

#ifdef DEBUG
#include <cassert>
#include <iostream>
#endif // DEBUG

// ===================================================================
// 型エイリアス
// ===================================================================
using Vector2 = DirectX::SimpleMath::Vector2;
using Vector3 = DirectX::SimpleMath::Vector3;
using Vector4 = DirectX::SimpleMath::Vector4;
using Matrix = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;
using Color = DirectX::SimpleMath::Color;

// ===================================================================
// 頂点情報
// ===================================================================
struct Vertex
{
	Vector3 Position;// 座標
	Vector4 Color;   // 色 (r, g, b, a)
	Vector2 TexCoord;// テクスチャ座標 (u, v)
};

// ===================================================================
// 描画レイヤー
// ===================================================================
enum class RenderLayer
{
	WORLD = 0,      // 3D空間（カメラ依存）

	UI = 100,       // 2D UI（カメラ非依存）
	UI_2,				// 2D UI　レイヤーわけ用に追加
	UI_Message,	// 2D UI（カメラ非依存）
	UI_BackGround,	// 2D UI（カメラ非依存）

	// 将来的な拡張用
	// PARTICLE = 50,
	// OVERLAY = 150,
};