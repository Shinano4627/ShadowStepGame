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
	m_status.state = UnitState::UnitWait;
	m_status.model = UnitModel::UnitNormal;
	m_status.hp = 5;
}

// ===================================================================
// 更新
// ・自ターン中のみ処理
// /現状プレイヤーのみ入力受付
// ===================================================================
void UnitComponent::Update()
{
	if (m_status.state != UnitState::UnitActive)
		return;

	if (m_status.camp == UnitCamp::UnitPlayer)
	{
		//仮実装：エンターでターン終了
		if (IO_MANAGER.GetKeyDown(TYPE_OK))
		{
			std::cout << "[Player] End Turn" << std::endl;
			EndTurn();
		}
	}
}

// ===================================================================
// ターン開始
// ===================================================================
void UnitComponent::BeginTurn()
{
	if (m_status.state == UnitState::UnitDown)
	{
		m_status.state = UnitState::UnitWait;
		return;
	}

	m_status.state = UnitState::UnitActive;

	std::cout << "[BeginTurn]"
		<< (m_status.camp == UnitCamp::UnitPlayer ? "Player" : "Enemy")
		<< std::endl;

	//仮仕様：エネミーは即ターン終了
	if (m_status.camp == UnitCamp::UnitEnemy)
	{
		EndTurn();
	}
}

// ===================================================================
// ターン終了
// ===================================================================
void UnitComponent::EndTurn()
{
	if (m_status.state != UnitState::UnitActive)
		return;

	m_status.state = UnitState::UnitWait;

	std::cout << "[EndTurn]"
		<< (m_status.camp == UnitCamp::UnitPlayer ? "Player" : "Enemy")
		<< std::endl;
}

// ===================================================================
// ターン終了判定
// ===================================================================
bool UnitComponent::HasFinishedTurn() const
{
	return m_status.state == UnitState::UnitWait;
}

// ===================================================================
// 行動可能判定
// ===================================================================
bool UnitComponent::CanAct() const
{
	return m_status.state == UnitState::UnitActive;
}

