#include "testTurnManager.h"
#include "UnitComponent.h"

void TurnManager::RegisterUnit(UnitComponent* unit)
{
    if (!unit) return;
    m_units.push_back(unit);
}

void TurnManager::StartBattle()
{
    if (m_units.empty()) return;

    m_currentIndex = 0;
    StartCurrentTurn();
}

void TurnManager::Update()
{
    if (m_units.empty() || m_currentIndex < 0)
        return;

    UnitComponent* current = m_units[m_currentIndex];
    if (!current) return;

    // 行動終了していたら次へ
    if (current->HasFinishedTurn())
    {
        NextTurn();
    }
}

bool TurnManager::IsCurrentUnit(const UnitComponent* unit) const
{
    if (m_currentIndex < 0 || m_currentIndex >= (int)m_units.size())
        return false;

    return m_units[m_currentIndex] == unit;
}

void TurnManager::StartCurrentTurn()
{
    UnitComponent* unit = m_units[m_currentIndex];
    if (!unit) return;

    unit->StartTurn();
}

void TurnManager::NextTurn()
{
    m_currentIndex++;

    if (m_currentIndex >= (int)m_units.size())
    {
        // 最後まで行ったら先頭へ（簡易）
        m_currentIndex = 0;
    }

    StartCurrentTurn();
}
