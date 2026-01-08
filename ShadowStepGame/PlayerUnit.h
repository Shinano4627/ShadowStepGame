#pragma once
#include "Unit.h"

//---------------------
// ・プレイヤーユニット基底クラス
// ・陣営をplayerに設定
// ・入力操作によって行動する
//---------------------

class PlayerUnit :public Unit
{
public:
	PlayerUnit();
	virtual ~PlayerUnit() = default;

	//更新処理
	//・入力受付
	//・選択状態の管理
	//・行動実行のトリガー
	void Update() override;

protected:
	//--入力・選択関連--//
};