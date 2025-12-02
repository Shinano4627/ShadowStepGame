// GameObject/PhysicsComponent/Rigidbody.cpp
#include "Rigidbody.h"
#include "GameObject.h"
#include "Transform.h"
#include "Game.h"
#include "PhysicsManager.h"

// ===================================================================
// コンストラクタ
// ===================================================================
Rigidbody::Rigidbody()
    : m_Velocity(Vector3::Zero)
    , m_Acceleration(Vector3::Zero)
    , m_Force(Vector3::Zero)
    , m_Mass(1.0f)
    , m_Drag(0.0f)
    , m_AngularDrag(0.05f)
    , m_UseGravity(true)
    , m_IsKinematic(false)
    , m_AngularVelocity(Vector3::Zero)
    , m_Torque(Vector3::Zero)
{
}

// ===================================================================
// 初期化
// ===================================================================
void Rigidbody::Init()
{
    // 必要に応じて初期化処理
}

// ===================================================================
// 更新処理
// ===================================================================
void Rigidbody::Update()
{
    // キネマティックなら物理演算をスキップ
    if (m_IsKinematic) return;

    // デルタタイム取得
    float deltaTime = Game::GetDeltaTime();

    // 力を統合 → 速度を更新
    IntegrateForces(deltaTime);

    // 速度を統合 → 位置を更新
    IntegrateVelocity(deltaTime);

    // 力とトルクをリセット
    m_Force = Vector3::Zero;
    m_Torque = Vector3::Zero;
}

// ===================================================================
// 終了処理
// ===================================================================
void Rigidbody::Uninit()
{
    // 特に処理なし
}

// ===================================================================
// 力を加える
// ===================================================================
void Rigidbody::AddForce(const Vector3& force)
{
    if (m_IsKinematic) return;
    m_Force += force;
}

// ===================================================================
// 衝撃を加える（即座に速度変更）
// ===================================================================
void Rigidbody::AddImpulse(const Vector3& impulse)
{
    if (m_IsKinematic) return;
    m_Velocity += impulse / m_Mass;
}

// ===================================================================
// トルクを加える
// ===================================================================
void Rigidbody::AddTorque(const Vector3& torque)
{
    if (m_IsKinematic) return;
    m_Torque += torque;
}

// ===================================================================
// 力の統合（力→加速度→速度）
// ===================================================================
void Rigidbody::IntegrateForces(float deltaTime)
{
    // 1. 力を加速度に変換 (F = ma → a = F/m)
    m_Acceleration = m_Force / m_Mass;

    // 2. 重力を加算
    if (m_UseGravity)
    {
        m_Acceleration += PHYSICS_MANAGER.GetGravity();
    }

    // 3. 加速度から速度を計算 (v = v0 + a×dt)
    m_Velocity += m_Acceleration * deltaTime;

    // 4. 空気抵抗を適用
    m_Velocity *= (1.0f - m_Drag);

    // 5. 角速度を更新（簡易版）
    // 本来は慣性テンソルを使うが、今回は簡易実装
    m_AngularVelocity += m_Torque * deltaTime;
    m_AngularVelocity *= (1.0f - m_AngularDrag);
}

// ===================================================================
// 速度の統合（速度→位置）
// ===================================================================
void Rigidbody::IntegrateVelocity(float deltaTime)
{
    if (!GetOwner()) return;

    Transform& transform = GetOwner()->GetTransform();

    // 1. 位置を更新 (p = p0 + v×dt)
    Vector3 position = transform.GetPosition();
    position += m_Velocity * deltaTime;
    transform.SetPosition(position);

    // 2. 回転を更新（簡易版）
    if (m_AngularVelocity.Length() > 0.001f)
    {
        Vector3 rotation = transform.GetRotation();
        rotation += m_AngularVelocity * deltaTime;
        transform.SetRotation(rotation);
    }
}