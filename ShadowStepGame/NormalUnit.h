#pragma once
#include "Unit.h"

//---------------------
//通常ユニット
//・移動のみ可能
//---------------------

class NormalUnit :public Unit
{
public:
	NormalUnit();
	virtual ~NormalUnit();

	//更新処理
	//・待機アニメーション
	//・選択時演出など
	void Update() override;
};