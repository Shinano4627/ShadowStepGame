#pragma once
#include "Unit.h"

//---------------------
// 攻撃ユニット
// ・unitを継承
// ・攻撃可能判定と攻撃処理を持つ
//---------------------

class AttackUnit :public Unit
{
public:
	AttackUnit();			//コンストラクタ
	virtual ~AttackUnit();	//デストラクタ

	//--攻撃関連--//
	//攻撃可能判定
	virtual bool CanAttack(const Unit* target) const;
	//攻撃実行
	virtual void Attack(Unit* target);

	//攻撃範囲取得
	int GetAttackRange() const { return m_attackRange; }
	
protected:
	int m_attackRange = 1;	//攻撃可能距離
};