#include "EnemyUnit.h"

//---------------------
// コンストラクタ
//---------------------
EnemyUnit::EnemyUnit()
{
	//敵陣営に設定
	m_camp = UnitCamp::Enemy;
}

//---------------------
// 更新処理
//---------------------
void EnemyUnit::Update()
{
	Unit::Update();

	//・TODO:
	//・行動可能ならAIによる行動決定
	//・CanAct()を通して攻撃制御
	//・移動/攻撃/配置などを選択
}