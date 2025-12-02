// ===================================================================
// PlayerMoverComponent.h
// プレイヤーの移動を制御するコンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"

class PlayerMoverComponent : public Component
{
private:
    float m_MoveSpeed;      // 移動速度
    float m_RotationSpeed;  // 回転速度

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    PlayerMoverComponent(float moveSpeed = 5.0f, float rotationSpeed = 3.0f)
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

        if (IO_MANAGER.GetKeyPressKeyBord(VK_W))
        {
            moveDirection.z += 1.0f; // 前進
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_S))
        {
            moveDirection.z -= 1.0f; // 後退
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_A))
        {
            moveDirection.x -= 1.0f; // 左移動
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_D))
        {
            moveDirection.x += 1.0f; // 右移動
        }

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

        // 回転入力（Q/E）
        DirectX::SimpleMath::Vector3 rotation = transform.GetRotation();

        if (IO_MANAGER.GetKeyPressKeyBord(VK_Q))
        {
            rotation.y -= m_RotationSpeed * deltaTime; // 左回転
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_E))
        {
            rotation.y += m_RotationSpeed * deltaTime; // 右回転
        }

        transform.SetRotation(rotation);
    }

    // ===================================================================
    // 速度設定
    // ===================================================================
    void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    float GetMoveSpeed() const { return m_MoveSpeed; }

    void SetRotationSpeed(float speed) { m_RotationSpeed = speed; }
    float GetRotationSpeed() const { return m_RotationSpeed; }
};