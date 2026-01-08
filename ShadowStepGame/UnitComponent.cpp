#include "UnitComponent.h"

void UnitComponent::Update()
{
    // 今は特になし。将来的に状態アニメーション等追加
}

// ターン開始
void UnitComponent::StartTurn()
{
    if (m_disabledThisTurn)
    {
        m_state = UnitState::Disabled;
        m_hp = m_maxHp;        // HP回復（プロトタイプ）
        m_disabledThisTurn = false;
        m_isMyTurn = false;
        return;
    }

    m_state = UnitState::Idle;
    m_isMyTurn = true;
}

// ターン終了
void UnitComponent::EndTurn()
{
    if (m_state == UnitState::Done)
        return;

    m_state = UnitState::Done;
    m_isMyTurn = false;
}

bool UnitComponent::IsMyTurn() const
{
    return m_state != UnitState::Disabled && m_isMyTurn;
}

bool UnitComponent::HasFinishedTurn() const
{
    return m_state == UnitState::Done;
}

bool UnitComponent::CanAct() const
{
    return m_state == UnitState::Idle;
}

void UnitComponent::TakeDamage(int damage)
{
    m_hp -= damage;
    if (m_hp <= 0)
    {
        m_hp = 0;
        Disable();
    }
}

void UnitComponent::Disable()
{
    m_disabledThisTurn = true;
    m_state = UnitState::Disabled;
}
