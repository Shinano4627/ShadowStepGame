// ===================================================================
// CameraRotatorComponent.h
// カメラを矢印キーで回転させるコンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"

class CameraRotatorComponent : public Component
{
private:
    Camera* m_Camera;                           // 制御するカメラ
    GameObject* m_Target;                       // 注視するターゲット
    float m_Distance;                           // ターゲットからの距離
    float m_Height;                             // 高さ
    float m_Angle;                              // 水平角度（ラジアン）
    float m_RotationSpeed;                      // 回転速度

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    CameraRotatorComponent(
        Camera* camera,
        GameObject* target,
        float distance = 10.0f,
        float height = 5.0f,
        float rotationSpeed = 2.0f)
        : m_Camera(camera)
        , m_Target(target)
        , m_Distance(distance)
        , m_Height(height)
        , m_Angle(0.0f)
        , m_RotationSpeed(rotationSpeed)
    {
    }

    // ===================================================================
    // 初期化
    // ===================================================================
    void Init() override
    {
        UpdateCamera();
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_Camera || !m_Target) return;

        float deltaTime = Game::GetDeltaTime();

        // 矢印キーで回転
        if (IO_MANAGER.GetKeyPressKeyBord(VK_LEFT))
        {
            m_Angle -= m_RotationSpeed * deltaTime; // 左回転
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_RIGHT))
        {
            m_Angle += m_RotationSpeed * deltaTime; // 右回転
        }

        // 上下矢印で距離調整（オプション）
        if (IO_MANAGER.GetKeyPressKeyBord(VK_UP))
        {
            m_Distance -= 5.0f * deltaTime; // 近づく
            if (m_Distance < 3.0f) m_Distance = 3.0f; // 最小距離
        }
        if (IO_MANAGER.GetKeyPressKeyBord(VK_DOWN))
        {
            m_Distance += 5.0f * deltaTime; // 遠ざかる
            if (m_Distance > 30.0f) m_Distance = 30.0f; // 最大距離
        }

        UpdateCamera();
    }

    // ===================================================================
    // カメラ更新
    // ===================================================================
    void UpdateCamera()
    {
        if (!m_Camera || !m_Target) return;

        using namespace DirectX::SimpleMath;

        // ターゲット位置取得
        Vector3 targetPosition = m_Target->GetTransform().GetPosition();

        // カメラ位置を計算（円周上を回転）
        float x = targetPosition.x + sin(m_Angle) * m_Distance;
        float z = targetPosition.z + cos(m_Angle) * m_Distance;
        float y = targetPosition.y + m_Height;

        Vector3 cameraPosition(x, y, z);

        // カメラ設定
        m_Camera->SetPosition(cameraPosition);
        m_Camera->SetTarget(targetPosition);
    }

    // ===================================================================
    // セッター
    // ===================================================================
    void SetDistance(float distance) { m_Distance = distance; }
    void SetHeight(float height) { m_Height = height; }
    void SetRotationSpeed(float speed) { m_RotationSpeed = speed; }
    void SetAngle(float angle) { m_Angle = angle; }

    float GetDistance() const { return m_Distance; }
    float GetHeight() const { return m_Height; }
    float GetRotationSpeed() const { return m_RotationSpeed; }
    float GetAngle() const { return m_Angle; }
};