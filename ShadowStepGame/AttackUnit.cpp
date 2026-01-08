#include "AttackUnit.h"

//---------------------
// コンストラクタ
//---------------------
AttackUnit::AttackUnit()
{
	m_type = UnitType::Attack;	//ユニット種類を設定
	m_attackRange = 1;//初期攻撃範囲(仮)
}

//---------------------
// デストラクタ
//---------------------
AttackUnit::~AttackUnit()
{

}

//---------------------
// 攻撃可能か判定
//---------------------
bool AttackUnit::CanAttack(const Unit* target) const
{
	//基本条件：ユニットが行動可能(Idle状態)
	if (!CanAct())
		return false;

	if (!target)
		return false;

	//同陣営は攻撃不可
	if (target->GetCamp() == m_camp)
		return false;

	//行動不能ユニットは攻撃不可
	if (target->GetState() == UnitState::Disabled)
		return false;

	//距離判定
	int dist = CalcDistanceFromSelf(target->GetGridPos());
	if (dist > m_attackRange)
		return false;

	return true;
}

//---------------------
// 攻撃処理
//---------------------
void AttackUnit::Attack(Unit* target)
{
	if (!CanAttack(target))	//行動可能か
		return;

	m_state = UnitState::Acting;	//攻撃中に設定

	//TODO:
	//・攻撃演出
	//・エフェクト
	//・SE再生など

	//攻撃成功
	target->Disable();


	m_state = UnitState::Done;
}