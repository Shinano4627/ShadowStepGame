#pragma once
#include "Component.h"
#include "Camera.h"
#include "IOManager.h"
#include "SimpleMath.h"
#include "GameSystemComponent.h"

using namespace DirectX::SimpleMath;

class OrbitCameraComponent : public Component
{
private:
    // 設定データ
    const int m_MoveFrame = 30; // 移動完了フレーム

public:
    OrbitCameraComponent(Camera* camera)
        : m_Camera(camera)
    {}

    void SetRotationSpeed(float speed) { m_RotSpeed = speed; }

    void Update()
    {
        if (!m_Camera) return;

        // -------------------------------------------------
        // 戦術視点かどうか（GameSystemに完全依存）
        // -------------------------------------------------
        bool isTacticalView =
            (m_GameSystem && m_GameSystem->IsSelectingPosition());

        // -------------------------------------------------
        // 戦術視点に「入った瞬間」だけスナップ
        // -------------------------------------------------
        bool enterTacticalView =
            isTacticalView && !m_WasTacticalView;

        if (enterTacticalView)
        {
            Vector3 unitPos(0.0f, 0.0f, 0.0f);

            if (m_GameSystem)
            {
                MapPosition mapPos = m_GameSystem->GetUnitPosition();
                unitPos.x = mapPos.x * 5.0f;
                unitPos.z = mapPos.z * 5.0f;
            }

            Vector3 tacticalDir(0.0f, 1.0f, -1.2f);
            tacticalDir.Normalize();

            float tacticalDistance = 50.0f;
            Vector3 camPos = unitPos + tacticalDir * tacticalDistance;

            m_Camera->SetTarget(unitPos);
            m_Camera->SetPosition(camPos);
        }

        // -------------------------------------------------
        // 戦術視点中は回転処理を完全にスキップ
        // -------------------------------------------------
        if (isTacticalView)
        {
            m_WasTacticalView = true;
            //return; // ← OrbitCamera処理を止める
        }
        else
        {
            m_WasTacticalView = false;
        }

        // -------------------------------------------------
        // 通常の OrbitCamera 処理
        // -------------------------------------------------
        Vector3 target = m_Camera->GetTarget();
        Vector3 camPos = m_Camera->GetPosition();

        Vector3 offset = camPos - target;
        float distance = offset.Length();
        if (distance < 0.001f) distance = 1.0f;
        offset.Normalize();

        // -----------------------------
        // 上下回転 (W / S)
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
        // 左右回転 (A / D)
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
        // カメラ位置反映
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

    bool    m_IsLocked = false;
    Vector3 m_LockTarget;
    Vector3 m_LockOffset;
    float   m_LockDistance;
    bool m_WasTacticalView = false;   // 戦術視点だったか（進入検出用）


    GameSystemComponent::BattleState m_PrevState =
        GameSystemComponent::BattleState::Init;

};