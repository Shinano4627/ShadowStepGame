// ===================================================================
// SceneProto.h
// Plese Write scene explanation
// ===================================================================
#pragma once
#include "SceneBase.h"

class SceneProto :public SceneBase
{
#pragma region 変数
public:

private:
	const int m_MaxSunMoveTurn = 9;	// 何ターンかけて太陽を昇る→沈むまで移動するか

#pragma endregion

#pragma region 関数
public:
	void Init() override;
	void UnInit() override;
	void Update() override;
	void Draw() override;
	void Draw(Camera*) override;

private:

#pragma endregion
};

