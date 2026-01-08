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
	//--AI関連--//
};