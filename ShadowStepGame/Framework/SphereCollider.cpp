// ===================================================================
// SphereCollider.cpp
// 球形コライダーの実装
// ===================================================================
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "GameObject.h"
#include <algorithm>

// ===================================================================
// コンストラクタ
// ===================================================================
SphereCollider::SphereCollider(float radius)
    : m_Radius(radius)
{
    m_Type = ColliderType::SPHERE;
}

// ===================================================================
// 衝突判定（メインエントリーポイント）
// ===================================================================
bool SphereCollider::CheckCollision(Collider* other, CollisionInfo& info)
{
    if (!other || !other->IsEnabled())
    {
        return false;
    }

    // コライダータイプに応じて適切な判定関数を呼び出す
    switch (other->GetType())
    {
    case ColliderType::SPHERE:
        return CheckSphere(static_cast<SphereCollider*>(other), info);

    case ColliderType::AABB:
        return CheckAABB(static_cast<AABBCollider*>(other), info);

    default:
        return false;
    }
}

// ===================================================================
// 球 vs 球 の衝突判定
// ===================================================================
bool SphereCollider::CheckSphere(SphereCollider* other, CollisionInfo& info)
{
    // 両球の中心位置を取得
    Vector3 center1 = GetWorldCenter();
    Vector3 center2 = other->GetWorldCenter();

    // 中心間のベクトル
    Vector3 delta = center2 - center1;
    float distanceSq = delta.LengthSquared();

    // 半径の合計
    float radiusSum = m_Radius + other->GetRadius();
    float radiusSumSq = radiusSum * radiusSum;

    // 距離の2乗が半径の合計の2乗より小さければ衝突
    if (distanceSq < radiusSumSq)
    {
        float distance = sqrtf(distanceSq);

        // 衝突情報を設定
        info.other = other->GetOwner();
        info.otherCollider = other;
        info.penetrationDepth = radiusSum - distance;

        // 距離がほぼゼロの場合（完全に重なっている）
        if (distance < Physics::COLLISION_EPSILON)
        {
            info.contactNormal = Vector3(0, 1, 0);  // 上方向をデフォルトに
            info.contactPoint = center1;
        }
        else
        {
            // 法線ベクトル（center1 から center2 へ向かう方向）
            info.contactNormal = delta / distance;

            // 衝突点（2つの球の接触点）
            info.contactPoint = center1 + info.contactNormal * m_Radius;
        }

        return true;
    }

    return false;
}

// ===================================================================
// 球 vs AABB の衝突判定
// ===================================================================
bool SphereCollider::CheckAABB(AABBCollider* other, CollisionInfo& info)
{
    // 球の中心
    Vector3 sphereCenter = GetWorldCenter();

    // AABBの最小・最大点
    Vector3 aabbMin = other->GetMin();
    Vector3 aabbMax = other->GetMax();

    // AABBに最も近い点を求める（クランプ）
    Vector3 closestPoint;
    closestPoint.x = (std::max)(aabbMin.x, (std::min)(sphereCenter.x, aabbMax.x));
    closestPoint.y = (std::max)(aabbMin.y, (std::min)(sphereCenter.y, aabbMax.y));
    closestPoint.z = (std::max)(aabbMin.z, (std::min)(sphereCenter.z, aabbMax.z));

    // 最近点と球の中心との距離
    Vector3 delta = closestPoint - sphereCenter;
    float distanceSq = delta.LengthSquared();

    // 距離が半径より小さければ衝突
    float radiusSq = m_Radius * m_Radius;
    if (distanceSq < radiusSq)
    {
        float distance = sqrtf(distanceSq);

        // 衝突情報を設定
        info.other = other->GetOwner();
        info.otherCollider = other;
        info.penetrationDepth = m_Radius - distance;
        info.contactPoint = closestPoint;

        // 法線ベクトル
        if (distance < Physics::COLLISION_EPSILON)
        {
            // 球の中心がAABB内部にある場合
            // 最も近い面の方向を法線とする
            Vector3 aabbCenter = (aabbMin + aabbMax) * 0.5f;
            Vector3 toCenter = sphereCenter - aabbCenter;

            // 各軸での距離を計算
            float dx = std::abs(toCenter.x);
            float dy = std::abs(toCenter.y);
            float dz = std::abs(toCenter.z);

            // 最も距離が小さい軸を選択
            if (dx < dy && dx < dz)
                info.contactNormal = Vector3(toCenter.x > 0 ? 1.0f : -1.0f, 0, 0);
            else if (dy < dz)
                info.contactNormal = Vector3(0, toCenter.y > 0 ? 1.0f : -1.0f, 0);
            else
                info.contactNormal = Vector3(0, 0, toCenter.z > 0 ? 1.0f : -1.0f);
        }
        else
        {
            // 通常の法線（球の中心から最近点へ向かう方向）
            info.contactNormal = delta / distance;
        }

        return true;
    }

    return false;
}

// ===================================================================
// デバッグ描画
// ===================================================================
#ifdef DEBUG
void SphereCollider::DebugDraw()
{
    // デバッグ用の球描画（後で実装）
    // 現在は何もしない
    // TODO: ワイヤーフレーム球を描画する関数を実装

    /*
    Vector3 center = GetWorldCenter();
    Color color = m_IsTrigger ? Color(1, 1, 0, 1) : Color(0, 1, 0, 1);
    DrawWireSphere(center, m_Radius, color);
    */
}
#endif