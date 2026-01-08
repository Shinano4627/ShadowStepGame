#pragma once
#include "Unit.h"

//--------------------
//巨大ユニット
//--------------------

class GiantUnit :public Unit
{
public:
	GiantUnit();
	virtual ~GiantUnit();

	//--巨人固有能力--//
	//壁を破壊できるか判定
	virtual bool CanBreakWall(const XMINT2& wallGridPos) const;

	//壁破壊処理
	virtual void BreakWall(const XMINT2& wallGridPos);

	//サイズ取得
	const XMINT2& GetSize() const { return m_size; }

protected:
	XMINT2 m_size;			//占有サイズ
	int m_wallBreakRange;	//壁破壊可能距離
};