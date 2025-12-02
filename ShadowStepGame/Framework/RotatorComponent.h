// ===================================================================
// RotatorComponent.h
// GameObjectを自動回転させるコンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Game.h"

class RotatorComponent : public Component
{
private:
    DirectX::SimpleMath::Vector3 m_RotationSpeed; // 回転速度（ラジアン/秒）

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    RotatorComponent(const DirectX::SimpleMath::Vector3& rotationSpeed = DirectX::SimpleMath::Vector3(0, 1, 0))
        : m_RotationSpeed(rotationSpeed)
    {
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_pOwner) return;

        // デルタタイム取得
        float deltaTime = Game::GetDeltaTime();

        // 現在の回転を取得
        DirectX::SimpleMath::Vector3 rotation = m_pOwner->GetTransform().GetRotation();

        // 回転を加算
        rotation += m_RotationSpeed * deltaTime;

        // 回転を設定
        m_pOwner->GetTransform().SetRotation(rotation);
    }

    // ===================================================================
    // 回転速度設定
    // ===================================================================
    void SetRotationSpeed(const DirectX::SimpleMath::Vector3& speed)
    {
        m_RotationSpeed = speed;
    }

    const DirectX::SimpleMath::Vector3& GetRotationSpeed() const
    {
        return m_RotationSpeed;
    }
};