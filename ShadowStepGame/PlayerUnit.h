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
	//プレイヤーの行動フェーズ
	enum class PlayerActionState
	{
		None,			//何もしていない
		SelectAction,	//行動選択中
		Executing		//行動実行中
	};

	PlayerActionState m_actionState;

	//各フェーズ処理
	void Update_SelectAction();
	void Update_Executing();

	//--入力・選択関連--//
};