#pragma once
#include "Unit.h"

//----------------------
//配置ユニット
//・オブジェクトを配置できる
//----------------------

class PlacementUnit :public Unit
{
public:
	PlacementUnit();
	virtual ~PlacementUnit();

	//--配置関連--//
	//配置可能判定
	virtual bool CanPlace(const XMINT2& grdPos) const;
	//配置実行
	virtual void PlaceObject(const XMINT2& gridPos);
	
	//配置範囲取得
	int GetPlaceRange() const { return m_placeRange; }

protected:
	int m_placeRange=1;	//配置可能距離(マス)
};