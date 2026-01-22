// ===================================================================
// SceneSelect.h
// Plese Write scene explanation
// ===================================================================
#pragma once
#include "SceneBase.h"

class SceneSelect :public SceneBase
{
#pragma region 変数
public:
	int m_CurrentSelected = 0;	// ラジオボタンの選択状況
	int m_MaxButton = 0;

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
	void MakeButton();
	void UpdateButton(int dir);
#pragma endregion
};

