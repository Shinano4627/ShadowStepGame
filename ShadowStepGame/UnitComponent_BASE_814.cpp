#include "UnitComponent.h"
#include "IOManager.h"
#include <iostream>
// ===================================================================
// コンストラクタ
// ===================================================================
UnitComponent::UnitComponent()
{
	//初期化
	m_status.camp = UnitCamp::UnitPlayer;
	m_status.model = UnitModel::UnitNormal;
	m_status.hp = 5;
}

// ===================================================================
// 更新
// ===================================================================
void UnitComponent::Update()
{
}