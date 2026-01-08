#pragma once
#include "Component.h"

enum class UnitCamp { Player, Enemy };
enum class UnitState { Idle, Acting, Done, Disabled };

class UnitComponent : public Component
{
private:
    // ユニット情報
    UnitCamp  m_camp = UnitCamp::Player;
    UnitState m_state = UnitState::Idle;
    int m_hp = 5;
    int m_maxHp = 5;

    // ターン管理
    bool m_isMyTurn = false;
    bool m_disabledThisTurn = false;

public:
    UnitComponent() = default;
    ~UnitComponent() = default;

    // 更新
    void Update() override;

    // ターン制御
    void StartTurn();
    void EndTurn();
    bool IsMyTurn() const;
    bool HasFinishedTurn() const;

    // 行動・状態
    bool CanAct() const;
    void TakeDamage(int damage);
    void Disable();

    // Getter / Setter
    UnitCamp GetCamp() const { return m_camp; }
    UnitState GetState() const { return m_state; }
    int GetHP() const { return m_hp; }
    void SetCamp(UnitCamp camp) { m_camp = camp; }
};
