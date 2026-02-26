#pragma once
#include "StateMachine.h"
#include <iostream>
#include "Component.h"

// コンポーネント
#include "MeshRendererComponent.h"

namespace UnitState
{
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

    // ステートマシンで管理するコンポーネント
    struct ManagedComponent
    {
        MeshRendererComponent* pUnitMeshRenderer;

        void operator= (const ManagedComponent& managedComponent)
        {
            pUnitMeshRenderer = managedComponent.pUnitMeshRenderer;
        }
    };

#pragma region ステート
    // ステータスクラスの基底クラス
    class IUnitState
    {
    protected:
        bool m_isChange = false;
    public:
        virtual void OnStart(ManagedComponent* managedComponent) = 0;
        virtual void OnUpdate(ManagedComponent* managedComponent) = 0;
        virtual void OnEnd(ManagedComponent* managedComponent) = 0;

        bool IsChange() { return m_isChange; };
    };

    class IdleState : public IUnitState
    {
    public:
        void OnStart(ManagedComponent* managedComponent) override
        {
            managedComponent->pUnitMeshRenderer->ChangeAnimation("Idle");
            managedComponent->pUnitMeshRenderer->SetDoAnimation(true);
        };
        void OnUpdate(ManagedComponent* managedComponent) override
        {};
        void OnEnd(ManagedComponent* managedComponent) override
        {};
    };

    class MoveState : public IUnitState
    {
    public:
        void OnStart(ManagedComponent* managedComponent) override
        {
            managedComponent->pUnitMeshRenderer->ChangeAnimation("Move");
            managedComponent->pUnitMeshRenderer->SetDoAnimation(true);
        };
        void OnUpdate(ManagedComponent* managedComponent) override
        {};
        void OnEnd(ManagedComponent* managedComponent) override
        {};
    };

    class AttackState : public IUnitState
    {
    public:
        void OnStart(ManagedComponent* managedComponent) override
        {
        };
        void OnUpdate(ManagedComponent* managedComponent) override
        {};
        void OnEnd(ManagedComponent* managedComponent) override
        {};
    };

    class PlaceState : public IUnitState
    {
    public:
        void OnStart(ManagedComponent* managedComponent) override
        {
        };
        void OnUpdate(ManagedComponent* managedComponent) override
        {};
        void OnEnd(ManagedComponent* managedComponent) override
        {};
    };

    class ShadowMoveState : public IUnitState
    {
    public:
        void OnStart(ManagedComponent* managedComponent) override
        {
        };
        void OnUpdate(ManagedComponent* managedComponent) override
        {};
        void OnEnd(ManagedComponent* managedComponent) override
        {};
    };

    class DamagedState : public IUnitState
    {
    public:
        void OnStart(ManagedComponent* managedComponent) override
        {
        };
        void OnUpdate(ManagedComponent* managedComponent) override
        {};
        void OnEnd(ManagedComponent* managedComponent) override
        {};
    };

    class DownState : public IUnitState
    {
    public:
        void OnStart(ManagedComponent* managedComponent) override
        {
        };
        void OnUpdate(ManagedComponent* managedComponent) override
        {};
        void OnEnd(ManagedComponent* managedComponent) override
        {};
    };
#pragma endregion
}

class UnitStateComponent : public Component
{
#pragma region 変数
private:
    StateMachine<UnitStateComponent, UnitState::UnitState> m_StateMachine;
    std::unordered_map<UnitState::UnitState, std::unique_ptr<UnitState::IUnitState>> m_StateTable;
    UnitState::UnitState m_PreState;    // ひとつ前

    UnitState::ManagedComponent m_ManagedComponent;
#pragma endregion

#pragma region 関数
public:
    UnitStateComponent()
        : m_StateMachine(*this)
    {
        // テーブルの初期化
        m_StateTable[UnitState::UnitState::Idle] = std::make_unique<UnitState::IdleState>();
        m_StateTable[UnitState::UnitState::Move] = std::make_unique<UnitState::MoveState>();
        m_StateTable[UnitState::UnitState::Attack] = std::make_unique<UnitState::AttackState>();
        m_StateTable[UnitState::UnitState::Place] = std::make_unique<UnitState::PlaceState>();
        m_StateTable[UnitState::UnitState::ShadowMove] = std::make_unique<UnitState::ShadowMoveState>();
        m_StateTable[UnitState::UnitState::Damaged] = std::make_unique<UnitState::DamagedState>();
        m_StateTable[UnitState::UnitState::Down] = std::make_unique<UnitState::DownState>();

        SetupStateMachine();
    }

    void SetInitData(const UnitState::ManagedComponent& managedComponent)
    {
        m_ManagedComponent = managedComponent;

        m_StateMachine.SetInitialState(UnitState::UnitState::Idle);
        m_PreState = UnitState::UnitState::Idle;
    }

    void Update()
    {
        m_StateMachine.Update();
    }

    void ChangeState(UnitState::UnitState state)
    {
        m_PreState = m_StateMachine.GetCurrentState();
        m_StateMachine.ChangeState(state);
    }

    UnitState::UnitState GetState()
    {
        return m_StateMachine.GetCurrentState();
    }

    UnitState::UnitState GetPreState()
    {
        return m_PreState;
    }

private:
    void SetupStateMachine()
    {
        for (const auto& state : m_StateTable)
        {
            UnitState::UnitState key = state.first;
            UnitState::ManagedComponent* arg = &m_ManagedComponent;

            m_StateMachine.RegisterState(key,
            {
            [key, arg](UnitStateComponent& p) { p.m_StateTable[key]->OnStart(arg); },    // onStart
            [key, arg](UnitStateComponent& p) { p.m_StateTable[key]->OnUpdate(arg); },     // onUpdate
            [key, arg](UnitStateComponent& p) { p.m_StateTable[key]->OnEnd(arg); }      // onEnd
            });
        }
    }
#pragma endregion
};
