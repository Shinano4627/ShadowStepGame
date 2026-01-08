#include "PlacementUnit.h"

//---------------------
// コンストラクタ
//---------------------
PlacementUnit::PlacementUnit()
{
	m_type = UnitType::Placement;	//ユニット種類設定
	m_placeRange = 1;				//配置範囲初期値
}

//---------------------
// デストラクタ
//---------------------
PlacementUnit::~PlacementUnit()
{

}

//---------------------
// 配置可能か判定
//---------------------
bool PlacementUnit::CanPlace(const XMINT2& gridPos) const
{
	//基本条件：行動可能状態
	if (!CanAct())
		return false;

	//距離判定
	int dist = CalcDistanceFromSelf(gridPos);
	if (dist > m_placeRange)
		return false;

	//TODO
	//・配置不可マス判定
	//・既にオブジェクトがあるか
	//・地形チェックなど

	return true;
}

//---------------------
// 配置実行
//---------------------
void PlacementUnit::PlaceObject(const XMINT2& gridPos)
{
	if (!CanPlace(gridPos))
		return;

	m_state = UnitState::Acting;

	//TODO
	//・オブジェクト生成
	//・フィールド/マップ管理クラスへ登録
	//・配置演出

	EndTurn();
}