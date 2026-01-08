#pragma once
#include "Unit.h"

//--------------------
// 敵ユニット基底クラス
// ・陣営をEnemyに設定
// ・AI制御(プロとタイプではplayerと同様操作予定？)
//--------------------

class EnemyUnit :public Unit
{
public:
	EnemyUnit();
	virtual ~EnemyUnit() = default;

	//更新処理
	//・AIによる行動決定
	//・志向中演出などを想定
	void Update() override;

private:
	//敵の行動フェーズ
	enum class EnemyActionState
	{
		None,			//何もしていない
		SelectAction,	//行動選択中
		Executing		//行動実行中
	};

	EnemyActionState m_actionState;

	//フェーズ別更新
	void Update_SelectAction();
	void Update_Executing();

	//--------------------
	// TODO:
	// 将来的にここにAI関連処理を追加
	// ・行動評価
	// ・ターゲット選択
	// ・移動先決定
	//--------------------
};