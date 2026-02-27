#pragma once
#include <unordered_map>
#include <functional>
#include <cassert>

template <typename Owner, typename State>
class StateMachine
{
    struct StateFuncs
    {
        std::function<void(Owner&)> onStart;
        std::function<void(Owner&)> onUpdate;
        std::function<void(Owner&)> onEnd;
    };

#pragma region 変数
private:
    Owner& m_owner;
    State m_currentState{};
    std::unordered_map<State, StateFuncs> m_states;
#pragma endregion

#pragma region 関数
public:
    explicit StateMachine(Owner& owner)
        : m_owner(owner)
    {}

    // ステート登録
    void RegisterState(State state, StateFuncs funcs)
    {
        m_states[state] = funcs;
    }

    // 初期ステート設定
    void SetInitialState(State state)
    {
        m_currentState = state;
        auto& funcs = m_states.at(state);
        if (funcs.onStart)
        {
            funcs.onStart(m_owner);
        }
    }

    // ステート遷移
    void ChangeState(State nextState)
    {
        if (m_currentState == nextState)
            return;

        auto& currentFuncs = m_states.at(m_currentState);
        if (currentFuncs.onEnd)
        {
            currentFuncs.onEnd(m_owner);
        }

        m_currentState = nextState;

        auto& nextFuncs = m_states.at(m_currentState);
        if (nextFuncs.onStart)
        {
            nextFuncs.onStart(m_owner);
        }
    }

    // 毎フレーム更新
    void Update()
    {
        auto& funcs = m_states.at(m_currentState);
        if (funcs.onUpdate)
        {
            funcs.onUpdate(m_owner);
        }
    }

    State GetCurrentState() const
    {
        return m_currentState;
    }
#pragma endregion
};
