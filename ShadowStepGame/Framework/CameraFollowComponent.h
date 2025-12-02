// ===================================================================
// CameraFollowComponent.h
// カメラがGameObjectを追従するコンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Camera.h"

class CameraFollowComponent : public Component
{
private:
    Camera* m_Camera;                           // 追従させるカメラ
    DirectX::SimpleMath::Vector3 m_Offset;      // オフセット（相対位置）
    float m_FollowSpeed;                        // 追従の滑らかさ（0-1、1で即座に追従）
    bool m_LookAtTarget;                        // ターゲットを注視するか

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    CameraFollowComponent(
        Camera* camera,
        const DirectX::SimpleMath::Vector3& offset = DirectX::SimpleMath::Vector3(0, 5, -10),
        float followSpeed = 0.1f,
        bool lookAtTarget = true)
        : m_Camera(camera)
        , m_Offset(offset)
        , m_FollowSpeed(followSpeed)
        , m_LookAtTarget(lookAtTarget)
    {
    }

    // ===================================================================
    // 初期化
    // ===================================================================
    void Init() override
    {
        // カメラの初期位置を設定
        if (m_Camera && m_pOwner)
        {
            UpdateCameraPosition();
        }
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_Camera || !m_pOwner) return;

        UpdateCameraPosition();
    }

    // ===================================================================
    // カメラ位置更新（内部処理）
    // ===================================================================
    void UpdateCameraPosition()
    {
        // ターゲット（オーナー）の位置を取得
        DirectX::SimpleMath::Vector3 targetPosition = m_pOwner->GetTransform().GetPosition();

        // 目標カメラ位置（ターゲット位置 + オフセット）
        DirectX::SimpleMath::Vector3 desiredPosition = targetPosition + m_Offset;

        // カメラの現在位置を取得
        DirectX::SimpleMath::Vector3 currentPosition = m_Camera->GetPosition();

        // 滑らかに追従（線形補間）
        DirectX::SimpleMath::Vector3 newPosition;
        if (m_FollowSpeed >= 1.0f)
        {
            // 即座に追従
            newPosition = desiredPosition;
        }
        else
        {
            // 滑らかに追従（Lerp）
            newPosition = DirectX::SimpleMath::Vector3::Lerp(currentPosition, desiredPosition, m_FollowSpeed);
        }

        // カメラ位置を設定
        m_Camera->SetPosition(newPosition);

        // 注視点設定（ターゲットを見る）
        if (m_LookAtTarget)
        {
            m_Camera->SetTarget(targetPosition);
        }
    }

    // ===================================================================
    // セッター
    // ===================================================================
    void SetOffset(const DirectX::SimpleMath::Vector3& offset) { m_Offset = offset; }
    void SetFollowSpeed(float speed) { m_FollowSpeed = speed; }
    void SetLookAtTarget(bool lookAt) { m_LookAtTarget = lookAt; }

    const DirectX::SimpleMath::Vector3& GetOffset() const { return m_Offset; }
    float GetFollowSpeed() const { return m_FollowSpeed; }
    bool IsLookAtTarget() const { return m_LookAtTarget; }
};