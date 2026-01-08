#include "EnemyUnit.h"

//---------------------
// コンストラクタ
//---------------------
EnemyUnit::EnemyUnit()
{
	//敵陣営に設定
	m_camp = UnitCamp::Enemy;
	m_actionState = EnemyActionState::None;
}

//---------------------
// 更新処理
//---------------------
void EnemyUnit::Update()
{
	Unit::Update();

	//自分のターンでなければ何もしない
	if (!IsMyTurn())
		return;

	//行動不能なら即ターン終了
	if (!CanAct())
	{
		EndTurn();
		return;
	}

	//ターン開始時
	if (m_actionState == EnemyActionState::None)
	{
		m_actionState = EnemyActionState::SelectAction;
	}

	switch (m_actionState)
	{
	case EnemyActionState::SelectAction:
		Update_SelectAction();
		break;

	case EnemyActionState::Executing:
		Update_Executing();
		break;
	}
}

void EnemyUnit::Update_SelectAction()
{
	//------------------
	// 仮実装：入力操作(プロトタイプ用)
	//------------------
	// TODO:
	// 将来的にAIによる行動選択へ差し替え
	// ・優先ターゲット選択
	// ・安全な移動先探索
	// ・攻撃可否判断
	//------------------

	bool decideMove = true;	//仮

	if (decideMove && CanMove())
	{
		XMINT2 targetPos = m_gridPos;	//仮移動
		MoveTo(targetPos);

		m_actionState = EnemyActionState::Executing;
		return;
	}

	//行動が決まらなければこのフェーズ継続
}

void EnemyUnit::Update_Executing()
{
	//TODO:
	//・行動アニメーション待ち
	//・SE/エフェクト完了待ち

	if (HasFinishedTurn())
	{
		EndTurn();
		m_actionState = EnemyActionState::None;
	}
}