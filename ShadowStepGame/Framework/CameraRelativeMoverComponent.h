// ===================================================================
// CameraRelativeMoverComponent.h
// カメラの向きに合わせた移動を行うコンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include "Camera.h"

class CameraRelativeMoverComponent : public Component
{
private:
    Camera* m_Camera;           // カメラへの参照
    float m_MoveSpeed;          // 移動速度
    float m_RotationSpeed;      // 回転速度

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    CameraRelativeMoverComponent(
        Camera* camera,
        float moveSpeed = 5.0f,
        float rotationSpeed = 3.0f)
        : m_Camera(camera)
        , m_MoveSpeed(moveSpeed)
        , m_RotationSpeed(rotationSpeed)
    {
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_pOwner || !m_Camera) return;

        float deltaTime = Game::GetDeltaTime();
        Transform& transform = m_pOwner->GetTransform();

        using namespace DirectX::SimpleMath;

        // ===================================================================
        // カメラの向きを取得
        // ===================================================================
        Vector3 cameraPosition = m_Camera->GetPosition();
        Vector3 targetPosition = m_Camera->GetTarget();

        // カメラの前方ベクトル（Y成分を無視して水平方向のみ）
        Vector3 cameraForward = targetPosition - cameraPosition;
        cameraForward.y = 0; // Y成分を0にして水平面に投影

        if (cameraForward.LengthSquared() > 0.0001f)
        {
            cameraForward.Normalize();
        }
        else
        {
            cameraForward = Vector3(0, 0, 1); // デフォルト方向
        }

        // カメラの右ベクトル（水平面上）
        Vector3 cameraRight = cameraForward.Cross(Vector3(0, 1, 0));
        if (cameraRight.LengthSquared() > 0.0001f)
        {
            cameraRight.Normalize();
        }
        else
        {
            cameraRight = Vector3(1, 0, 0); // デフォルト方向
        }

        // ===================================================================
        // カメラ基準の移動入力
        // ===================================================================
        Vector3 moveDirection(0, 0, 0);

        if (IO_MANAGER.GetKeyPressKeyBord(VK_W))
        {
            moveDirection += cameraForward; // カメラの前方へ
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_S))
        {
            moveDirection -= cameraForward; // カメラの後方へ
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_A))
        {
            moveDirection += cameraRight; // カメラの左へ
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_D))
        {
            moveDirection -= cameraRight; // カメラの右へ
        }

        // 移動方向を正規化して移動
        if (moveDirection.LengthSquared() > 0.0001f)
        {
            moveDirection.Normalize();

            // 現在の位置を取得
            Vector3 position = transform.GetPosition();

            // 移動を適用
            position += moveDirection * m_MoveSpeed * deltaTime;

            // 位置を設定
            transform.SetPosition(position);

            // ===================================================================
            // オプション：移動方向を向く（自動回転）
            // ===================================================================
            float targetAngle = atan2f(-moveDirection.x, -moveDirection.z);
            Vector3 rotation = transform.GetRotation();
            rotation.y = targetAngle;
            transform.SetRotation(rotation);
            
        }

        // ===================================================================
        // Q/Eで手動回転（オプション）
        // ===================================================================
        Vector3 rotation = transform.GetRotation();

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

    void SetCamera(Camera* camera) { m_Camera = camera; }
    Camera* GetCamera() const { return m_Camera; }
};