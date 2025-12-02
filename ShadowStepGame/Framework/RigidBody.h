#pragma once
#include "Component.h"
#include "SystemCommon.h"

class Rigidbody : public Component
{
private:
    // ===================================================================
    // 物理パラメータ
    // ===================================================================
    Vector3 m_Velocity;          // 速度
    Vector3 m_Acceleration;      // 加速度
    Vector3 m_Force;             // 累積された力

    float m_Mass;                // 質量
    float m_Drag;                // 空気抵抗 (0-1)
    float m_AngularDrag;         // 角速度減衰 (0-1)

    bool m_UseGravity;           // 重力を受けるか
    bool m_IsKinematic;          // キネマティックか

    Vector3 m_AngularVelocity;   // 角速度
    Vector3 m_Torque;            // トルク累積

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    Rigidbody();

    // ===================================================================
    // Component インターフェース実装
    // ===================================================================
    void Init() override;
    void Update() override;
    void Uninit() override;

    // ===================================================================
    // 力の操作
    // ===================================================================
    void AddForce(const Vector3& force);        // 力を加える
    void AddImpulse(const Vector3& impulse);    // 衝撃を加える
    void AddTorque(const Vector3& torque);      // トルクを加える

    // ===================================================================
    // Getter / Setter
    // ===================================================================
    void SetVelocity(const Vector3& velocity) { m_Velocity = velocity; }
    Vector3 GetVelocity() const { return m_Velocity; }

    void SetMass(float mass) { m_Mass = (mass > 0.0f) ? mass : 0.001f; }
    float GetMass() const { return m_Mass; }

    void SetDrag(float drag) { m_Drag = drag; }
    float GetDrag() const { return m_Drag; }

    void SetUseGravity(bool use) { m_UseGravity = use; }
    bool IsUseGravity() const { return m_UseGravity; }

    void SetKinematic(bool kinematic) { m_IsKinematic = kinematic; }
    bool IsKinematic() const { return m_IsKinematic; }

    void SetAngularVelocity(const Vector3& angularVelocity) { m_AngularVelocity = angularVelocity; }
    Vector3 GetAngularVelocity() const { return m_AngularVelocity; }

private:
    // ===================================================================
    // 内部処理
    // ===================================================================
    void IntegrateForces(float deltaTime);      // 力→加速度→速度
    void IntegrateVelocity(float deltaTime);    // 速度→位置
};