// ===================================================================
// SunManageComponent.h
// ステージ上の太陽の動きを制御するコンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
//#include "IOManager.h"
#include "Game.h"

class SunManageComponent : public Component
{
private:
    float m_MoveSpeed;      // 移動速度
    float m_RotationSpeed;  // 回転速度

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SunManageComponent(float moveSpeed = 5.0f, float rotationSpeed = 3.0f)
        : m_MoveSpeed(moveSpeed)
        , m_RotationSpeed(rotationSpeed)
    {
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_pOwner) return;

        float deltaTime = Game::GetDeltaTime();
        Transform& transform = m_pOwner->GetTransform();

        // 移動入力
        DirectX::SimpleMath::Vector3 moveDirection(0, 0, 0);

        // 移動方向を正規化
        if (moveDirection.LengthSquared() > 0)
        {
            moveDirection.Normalize();

            // 現在の位置を取得
            DirectX::SimpleMath::Vector3 position = transform.GetPosition();

            // 移動を適用
            position += moveDirection * m_MoveSpeed * deltaTime;

            // 位置を設定
            transform.SetPosition(position);
        }
    }

    // ===================================================================
    // 速度設定
    // ===================================================================
    void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    float GetMoveSpeed() const { return m_MoveSpeed; }

    void SetRotationSpeed(float speed) { m_RotationSpeed = speed; }
    float GetRotationSpeed() const { return m_RotationSpeed; }
};