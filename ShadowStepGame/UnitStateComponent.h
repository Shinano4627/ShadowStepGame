#include "StateMachine.h"
#include <iostream>
#include "Component.h"

// コンポーネント
#include "MeshRendererComponent.h"

// ステートマシン内で管理するコンポーネント
//extern MeshRendererComponent* g_pUnitMeshRenderer;

enum class UnitState
{
    Idle,
    Move,
    Attack,
    Place,
    ShadowMove,
    Damaged,
    Down,
};

#pragma region ステータス
// ステータスクラスの基底クラス
class IUnitState
{
protected:
    bool m_isChange = false;
public:
    virtual void OnStart() = 0;
    virtual void OnUpdate() = 0;
    virtual void OnEnd() = 0;

    bool IsChange() { return m_isChange; };
};

class IdleState : public IUnitState
{
public:
    void OnStart() override
    {
        //g_pUnitMeshRenderer->ChangeAnimation("Idle");
        //g_pUnitMeshRenderer->SetDoAnimation(true);
    };
    void OnUpdate() override
    {};
    void OnEnd() override
    {};
};

class MoveState : public IUnitState
{
public:
    void OnStart() override
    {
    };
    void OnUpdate() override
    {};
    void OnEnd() override
    {};
};

class AttackState : public IUnitState
{
public:
    void OnStart() override
    {
    };
    void OnUpdate() override
    {};
    void OnEnd() override
    {};
};

class PlaceState : public IUnitState
{
public:
    void OnStart() override
    {
    };
    void OnUpdate() override
    {};
    void OnEnd() override
    {};
};

class ShadowMoveState : public IUnitState
{
public:
    void OnStart() override
    {
    };
    void OnUpdate() override
    {};
    void OnEnd() override
    {};
};

class DamagedState : public IUnitState
{
public:
    void OnStart() override
    {
    };
    void OnUpdate() override
    {};
    void OnEnd() override
    {};
};

class DownState : public IUnitState
{
public:
    void OnStart() override
    {
    };
    void OnUpdate() override
    {};
    void OnEnd() override
    {};
};
#pragma endregion

class UnitStateComponent : public Component
{
#pragma region 変数
private:
    StateMachine<UnitStateComponent, UnitState> m_StateMachine;
    std::unordered_map<UnitState, std::unique_ptr<IUnitState>> m_StateTable;
    UnitState m_PreState;    // ひとつ前
#pragma endregion

#pragma region 関数
public:
    UnitStateComponent()
        : m_StateMachine(*this)
    {
        // テーブルの初期化
        m_StateTable[UnitState::Idle] = std::make_unique<IdleState>();
        m_StateTable[UnitState::Move] = std::make_unique<MoveState>();
        m_StateTable[UnitState::Attack] = std::make_unique<AttackState>();
        m_StateTable[UnitState::Place] = std::make_unique<PlaceState>();
        m_StateTable[UnitState::ShadowMove] = std::make_unique<ShadowMoveState>();
        m_StateTable[UnitState::Damaged] = std::make_unique<DamagedState>();
        m_StateTable[UnitState::Down] = std::make_unique<DownState>();

        SetupStateMachine();
        m_StateMachine.SetInitialState(UnitState::Idle);
        m_PreState = UnitState::Idle;
    }

    void Update()
    {
        m_StateMachine.Update();
    }

    void ChangeState(UnitState state)
    {
        m_PreState = m_StateMachine.GetCurrentState();
        m_StateMachine.ChangeState(state);
    }

    UnitState GetState()
    {
        return m_StateMachine.GetCurrentState();
    }

    UnitState GetPreState()
    {
        return m_PreState;
    }

    void Move()
    {
        std::cout << "Player Moving\n";
    }

    void Attack()
    {
        std::cout << "Player Attacking\n";
    }

private:
    void SetupStateMachine()
    {
        for (const auto& state : m_StateTable)
        {
            UnitState key = state.first;

            m_StateMachine.RegisterState(key,
            {
            [key](UnitStateComponent& p) { p.m_StateTable[key]->OnStart(); },    // onStart
            [key](UnitStateComponent& p) { p.m_StateTable[key]->OnUpdate(); },     // onUpdate
            [key](UnitStateComponent& p) { p.m_StateTable[key]->OnEnd(); }      // onEnd
            });
        }
    }
#pragma endregion
};
