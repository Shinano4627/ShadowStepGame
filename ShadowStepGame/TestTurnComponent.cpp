#include "TestTurnComponent.h"
#include <iostream>

// ===================================================================
// Unit登録
// ===================================================================
void TestTurnComponent::AddUnit(UnitComponent* unit)
{
	if (!unit) return;
	m_units.push_back(unit);
}

// ===================================================================
// 更新
// ・現在のユニットが行動終了したら次へ
// ===================================================================
void TestTurnComponent::Update()
{
	if (m_units.empty())
		return;

	//最初のターン開始
	if (!m_turnStarted)
	{
		m_currentIndex = -1;
		StartNextTurn();
		m_turnStarted = true;
		return;
	}

	//現在のユニットがターン終了したら次へ
	if (m_units[m_currentIndex]->HasFinishedTurn())
	{
		StartNextTurn();
	}
}

// ===================================================================
// 次のターンへ
// ===================================================================
void TestTurnComponent::StartNextTurn()
{
	//次のインデックスへ
	m_currentIndex++;
	if (m_currentIndex >= m_units.size())
	{
		m_currentIndex = 0;
	}

	m_units[m_currentIndex]->BeginTurn();
}