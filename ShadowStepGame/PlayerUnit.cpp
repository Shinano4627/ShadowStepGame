#include "PlayerUnit.h"

//---------------------
// コンストラクタ
//---------------------
PlayerUnit::PlayerUnit()
{
	//プレイヤー陣営に設定
	m_camp = UnitCamp::Player;
	m_actionState = PlayerActionState::None;
}

//---------------------
// 更新処理
//---------------------
void PlayerUnit::Update()
{
	Unit::Update();

	//自分のターンでなければ何もしない
	if (!IsMyTurn())
	{
		return;
	}

	//行動不能なら即ターン終了
	if (!CanAct())
	{
		EndTurn();
		return;
	}

	//ターン開始時
	if (m_actionState == PlayerActionState::None)
	{
		m_actionState = PlayerActionState::SelectAction;
	}

	//行動フェーズごとの処理
	switch(m_actionState)
	{
	case PlayerActionState::SelectAction:
		Update_SelectAction();
		break;

	case PlayerActionState::Executing:
		Update_Executing();
		break;
	}
}

//---------------------
// 行動選択フェーズ
//---------------------
void PlayerUnit::Update_SelectAction()
{
	//---------------------
    // 入力受付
    //---------------------

	//実際にはInput/UIクラスに切り出す？
	bool decideMove = false;
	bool decideAttack = false;
	bool decidePlace = false;

	decideMove = true; //仮ロジック

	//---------------------
	// UnitTypeに応じた分岐
	//---------------------

	if (decideMove && CanMove())
	{
		XMINT2 targetPos = m_gridPos;	//仮
		MoveTo(targetPos);

		m_actionState = PlayerActionState::Executing;
		return;
	}

	if (decideAttack && m_type == UnitType::Attack)
	{
		//AttackUnitに処理を委譲
		//static_cast<AttackUnit*>(this)->Attack(target);

		m_actionState = PlayerActionState::Executing;
		return;
	}

	if (decidePlace && m_type == UnitType::Placement)
	{
		//static_cast<PlacementUnit*>(this)->PlaceObject(pos);

		m_actionState = PlayerActionState::Executing;
		return;
	}

	//行動未決定の間このフェーズにとどまる
}

//---------------------
// 行動実行フェーズ
//---------------------
void PlayerUnit::Update_Executing()
{
	//基本方針：
	// ・Unit側の行動がDoneになるのを待つ

	if (HasFinishedTurn())
	{
		EndTurn();
		m_actionState = PlayerActionState::None;
	}
}