#include "TurnComponent.h"
#include "UnitComponent.h"

void TurnComponent::AddUnit(UnitComponent* unit)
{
    m_units.push_back(unit);
}

void TurnComponent::StartNextTurn()
{
    if (m_units.empty()) return;

    // 前のターンを終了
    if (m_currentIndex < m_units.size())
    {
        m_units[m_currentIndex]->EndTurn();
    }

    // 次のユニットへ
    m_currentIndex = (m_currentIndex + 1) % m_units.size();
    m_units[m_currentIndex]->StartTurn();
}

void TurnComponent::Update()
{
    if (m_units.empty()) return;

    UnitComponent* current = m_units[m_currentIndex];

    // 現在のユニットが行動完了なら次のターンへ
    if (current->HasFinishedTurn())
    {
        StartNextTurn();
    }
}
