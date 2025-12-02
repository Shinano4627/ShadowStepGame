// ===================================================================
// Collider.cpp
// 衝突判定コンポーネントの基底クラス実装
// ===================================================================
#include "Collider.h"
#include "GameObject.h"
#include "PhysicsManager.h"

// ===================================================================
// コンストラクタ
// ===================================================================
Collider::Collider()
    : m_Type(ColliderType::SPHERE)
    , m_Center(Vector3::Zero)
    , m_Offset(Vector3::Zero)
    , m_IsTrigger(false)
    , m_IsStatic(false)
    , m_Layer(0)
    , m_OnCollisionEnter(nullptr)
    , m_OnCollisionStay(nullptr)
    , m_OnCollisionExit(nullptr)
{
}

// ===================================================================
// 初期化
// ===================================================================
void Collider::Init()
{
    // PhysicsManagerに自動登録
    PHYSICS_MANAGER.RegisterCollider(this);
}

// ===================================================================
// 終了処理
// ===================================================================
void Collider::Uninit()
{
    // PhysicsManagerから自動解除
    PHYSICS_MANAGER.UnregisterCollider(this);
}

// ===================================================================
// ワールド座標での中心位置を取得
// ===================================================================
Vector3 Collider::GetWorldCenter() const
{
    if (!m_pOwner)
    {
        return Vector3::Zero;
    }

    // オーナーのワールド座標 + ローカル中心 + オフセット
    const Vector3& ownerPos = m_pOwner->GetTransform().GetPosition();
    return ownerPos + m_Center + m_Offset;
}

// ===================================================================
// 衝突開始イベント
// ===================================================================
void Collider::OnCollisionEnter(const CollisionInfo& info)
{
    if (m_OnCollisionEnter)
    {
        m_OnCollisionEnter(info);
    }
}

// ===================================================================
// 衝突中イベント
// ===================================================================
void Collider::OnCollisionStay(const CollisionInfo& info)
{
    if (m_OnCollisionStay)
    {
        m_OnCollisionStay(info);
    }
}

// ===================================================================
// 衝突終了イベント
// ===================================================================
void Collider::OnCollisionExit(const CollisionInfo& info)
{
    if (m_OnCollisionExit)
    {
        m_OnCollisionExit(info);
    }
}