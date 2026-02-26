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

    // カメラ移動用
    Vector3 m_StartCamPos;      // 移動開始時のカメラ位置
    Vector3 m_EndCamPos;        // 移動先のカメラ位置
    Vector3 m_StartTarget;      // 移動開始時のターゲット位置
    Vector3 m_EndTarget;        // 移動先のターゲット位置
    int     m_MoveFrameCount = 0; // 現在の移動フレームカウント
    bool    m_IsMoving = false;   // 移動中フラグ

public:
    OrbitCameraComponent(Camera* camera)
        : m_Camera(camera)
    {}

    void SetRotationSpeed(float speed) { m_RotSpeed = speed; }

    // カメラ移動開始
    void StartMoveTo(const Vector3& endCamPos, const Vector3& endTarget)
    {
        if (!m_Camera) return;
        m_StartCamPos      = m_Camera->GetPosition();
        m_StartTarget      = m_Camera->GetTarget();
        m_EndCamPos        = endCamPos;
        m_EndTarget        = endTarget;
        m_MoveFrameCount   = 0;
        m_IsMoving         = true;
    }

    // 移動が完了しているか
    bool IsMoveFinished() const { return !m_IsMoving; }

    void Update()
    {
        if (!m_Camera) return;

        // -------------------------------------------------
        // 戦術視点かどうか（GameSystemに完全依存）
        // -------------------------------------------------
        bool isTacticalView =
            (m_GameSystem && m_GameSystem->IsSelectingPosition());

        // -------------------------------------------------
        // カメラ移動中の補間処理
        // -------------------------------------------------
        if (m_IsMoving)
        {
            m_MoveFrameCount++;

            // 補間係数（0.0〜1.0）
            float t = static_cast<float>(m_MoveFrameCount) / static_cast<float>(m_MoveFrame);
            if (t >= 1.0f)
            {
                t = 1.0f;
                m_IsMoving = false;
            }

            // 線形補間でカメラ位置・ターゲットを更新
            Vector3 camPos   = Vector3::Lerp(m_StartCamPos,  m_EndCamPos,  t);
            Vector3 target   = Vector3::Lerp(m_StartTarget,  m_EndTarget,  t);
            m_Camera->SetPosition(camPos);
            m_Camera->SetTarget(target);

            // 移動中はOrbit処理をスキップ
            return;
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
        // リリースのときはカメラが地面の下にうまらないように調整
#ifdef _DEBUG
        if (camPos.y < 0.5f)
        {
            camPos.y = 0.5f;
        }
#endif // DEBUG

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