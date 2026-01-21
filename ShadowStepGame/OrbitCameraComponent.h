#pragma once
#include "Component.h"
#include "Camera.h"
#include "IOManager.h"
#include "SimpleMath.h"
#include "GameSystemComponent.h"

using namespace DirectX::SimpleMath;

class OrbitCameraComponent : public Component
{
public:
    OrbitCameraComponent(Camera* camera)
        : m_Camera(camera)
    {}

    void SetRotationSpeed(float speed) { m_RotSpeed = speed; }

    void Update() override
    {
        if (!m_Camera) return;

        // -----------------------------
        // ターゲット設定
        // -----------------------------
        Vector3 target(0, 0, 0); // デフォルト

        if (m_GameSystem)
        {
            auto state = m_GameSystem->GetBattleState();
            if (state == GameSystemComponent::BattleState::UnitActionSelect ||
                state == GameSystemComponent::BattleState::UnitActing ||
                state == GameSystemComponent::BattleState::UnitEnd ||
                state == GameSystemComponent::BattleState::UnitSelect)
            {
                // TODO: Unit 実装後に currentUnit の位置を取得
                target = Vector3(20, 20, 20);
                // auto* currentUnit = m_GameSystem->GetCurrentTimeline()->unit;
                // if (currentUnit) target = currentUnit->GetPosition();
            }
        }

        // -----------------------------
        // カメラ位置とオフセット
        // -----------------------------
        Vector3 camPos = m_Camera->GetPosition();
        Vector3 offset = camPos - target;
        float distance = offset.Length();
        if (distance < 0.001f) distance = 1.0f;
        offset.Normalize();

        // -----------------------------
        // 上下回転 (W/S)
        // -----------------------------
        if (IO_MANAGER.GetKeyPressKeyBord('W'))
        {
            Vector3 right = Vector3::Up.Cross(offset);
            right.Normalize();
            Matrix rot = Matrix::CreateFromAxisAngle(right, m_RotSpeed);
            offset = Vector3::TransformNormal(offset, rot);
        }
        if (IO_MANAGER.GetKeyPressKeyBord('S'))
        {
            Vector3 right = Vector3::Up.Cross(offset);
            right.Normalize();
            Matrix rot = Matrix::CreateFromAxisAngle(right, -m_RotSpeed);
            offset = Vector3::TransformNormal(offset, rot);
        }

        // -----------------------------
        // 左右回転 (A/D)
        // -----------------------------
        if (IO_MANAGER.GetKeyPressKeyBord('A'))
        {
            Matrix rot = Matrix::CreateFromAxisAngle(Vector3::Up, m_RotSpeed);
            offset = Vector3::TransformNormal(offset, rot);
        }
        if (IO_MANAGER.GetKeyPressKeyBord('D'))
        {
            Matrix rot = Matrix::CreateFromAxisAngle(Vector3::Up, -m_RotSpeed);
            offset = Vector3::TransformNormal(offset, rot);
        }

        // -----------------------------
        // カメラ位置更新
        // -----------------------------
        camPos = target + offset * distance;
        m_Camera->SetPosition(camPos);
        m_Camera->SetTarget(target);
    }

    void SetGameSystem(GameSystemComponent* gs) { m_GameSystem = gs; }

private:
    Camera* m_Camera = nullptr;
    GameSystemComponent* m_GameSystem = nullptr;
    float m_RotSpeed = 0.02f;
};