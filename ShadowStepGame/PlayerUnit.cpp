#include "PlayerUnit.h"

//---------------------
// コンストラクタ
//---------------------
PlayerUnit::PlayerUnit()
{
	//プレイヤー陣営に設定
	m_camp = UnitCamp::Player;
}

//---------------------
// 更新処理
//---------------------
void PlayerUnit::Update()
{
	Unit::Update();

	//TODO:
	//入力受付
	//・ユニット選択中の処理
	//・移動/攻撃/配置の実行
}