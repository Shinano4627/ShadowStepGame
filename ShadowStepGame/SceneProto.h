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

