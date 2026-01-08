#include "GiantUnit.h"

//---------------------
// コンストラクタ
//---------------------
GiantUnit::GiantUnit()
{
	m_type = UnitType::Giant;	//ユニット種類設定
	m_size = XMINT2(3, 3);		//占有サイズ

	//巨人用にパラメーター調整があればここに記述
	m_wallBreakRange = 1;	//壁破壊可能距離
}

//---------------------
// デストラクタ
//---------------------
GiantUnit::~GiantUnit()
{

}

//---------------------
// 壁破壊可能判定
//---------------------
bool GiantUnit::CanBreakWall(const XMINT2& wallGridPos) const
{
	if (!CanAct())
		return false;

	//距離判定
	int dist = CalcDistanceFromSelf(wallGridPos);
	if (dist > m_wallBreakRange)
		return false;

	//TODO:
	//・wallGridPosが壁か
	//・破壊可能な課bか
	//・マップ範囲チェック

	return true;
}

//---------------------
// 壁破壊実行
//---------------------
void GiantUnit::BreakWall(const XMINT2& wallGridPos)
{
	if (!CanBreakWall(wallGridPos))
		return;

	m_state = UnitState::Acting;

	//TODO:
	//・壁オブジェクト取得
	//・破壊処理
	//・SE/エフェクト
	//・マップ更新通知

	m_state = UnitState::Done;
}