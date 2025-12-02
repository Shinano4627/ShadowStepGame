// ===================================================================
// AABBCollider.cpp
// 軸並行境界ボックスコライダーの実装
// ===================================================================
#include "AABBCollider.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include <algorithm>

// ===================================================================
// コンストラクタ
// ===================================================================
AABBCollider::AABBCollider(const Vector3& size)
    : m_Size(size)
{
    m_Type = ColliderType::AABB;
}

// ===================================================================
// 最小点を取得（ワールド座標）
// ===================================================================
Vector3 AABBCollider::GetMin() const
{
    Vector3 center = GetWorldCenter();
    Vector3 halfSize = m_Size * 0.5f;
    return center - halfSize;
}

// ===================================================================
// 最大点を取得（ワールド座標）
// ===================================================================
Vector3 AABBCollider::GetMax() const
{
    Vector3 center = GetWorldCenter();
    Vector3 halfSize = m_Size * 0.5f;
    return center + halfSize;
}

// ===================================================================
// 衝突判定（メインエントリーポイント）
// ===================================================================
bool AABBCollider::CheckCollision(Collider* other, CollisionInfo& info)
{
    if (!other || !other->IsEnabled())
    {
        return false;
    }

    // コライダータイプに応じて適切な判定関数を呼び出す
    switch (other->GetType())
    {
    case ColliderType::AABB:
        return CheckAABB(static_cast<AABBCollider*>(other), info);

    case ColliderType::SPHERE:
        return CheckSphere(static_cast<SphereCollider*>(other), info);

    default:
        return false;
    }
}

// ===================================================================
// AABB vs AABB の衝突判定
// ===================================================================
bool AABBCollider::CheckAABB(AABBCollider* other, CollisionInfo& info)
{
    // 両AABBの最小・最大点を取得
    Vector3 min1 = GetMin();
    Vector3 max1 = GetMax();
    Vector3 min2 = other->GetMin();
    Vector3 max2 = other->GetMax();

    // 各軸で重なりをチェック
    bool collisionX = (min1.x <= max2.x) && (max1.x >= min2.x);
    bool collisionY = (min1.y <= max2.y) && (max1.y >= min2.y);
    bool collisionZ = (min1.z <= max2.z) && (max1.z >= min2.z);

    // すべての軸で重なっていれば衝突
    if (collisionX && collisionY && collisionZ)
    {
        // 各軸でのめり込み量を計算
        float overlapX = (std::min)(max1.x - min2.x, max2.x - min1.x);
        float overlapY = (std::min)(max1.y - min2.y, max2.y - min1.y);
        float overlapZ = (std::min)(max1.z - min2.z, max2.z - min1.z);

        // 最小のめり込み軸を選択（これが分離軸）
        float minOverlap = (std::min)({ overlapX, overlapY, overlapZ });

        // 衝突情報を設定
        info.other = other->GetOwner();
        info.otherCollider = other;
        info.penetrationDepth = minOverlap;

        // 法線ベクトルを決定（最小めり込み軸の方向）
        Vector3 center1 = GetWorldCenter();
        Vector3 center2 = other->GetWorldCenter();
        Vector3 direction = center2 - center1;

        if (minOverlap == overlapX)
        {
            // X軸が最小めり込み
            info.contactNormal = Vector3(direction.x > 0 ? 1.0f : -1.0f, 0, 0);
            info.contactPoint = Vector3(
                direction.x > 0 ? max1.x : min1.x,
                (min1.y + max1.y) * 0.5f,
                (min1.z + max1.z) * 0.5f
            );
        }
        else if (minOverlap == overlapY)
        {
            // Y軸が最小めり込み
            info.contactNormal = Vector3(0, direction.y > 0 ? 1.0f : -1.0f, 0);
            info.contactPoint = Vector3(
                (min1.x + max1.x) * 0.5f,
                direction.y > 0 ? max1.y : min1.y,
                (min1.z + max1.z) * 0.5f
            );
        }
        else
        {
            // Z軸が最小めり込み
            info.contactNormal = Vector3(0, 0, direction.z > 0 ? 1.0f : -1.0f);
            info.contactPoint = Vector3(
                (min1.x + max1.x) * 0.5f,
                (min1.y + max1.y) * 0.5f,
                direction.z > 0 ? max1.z : min1.z
            );
        }

        return true;
    }

    return false;
}

// ===================================================================
// AABB vs 球 の衝突判定
// ===================================================================
bool AABBCollider::CheckSphere(SphereCollider* other, CollisionInfo& info)
{
    // SphereColliderの判定を利用
    // （球 vs AABB は SphereCollider::CheckAABB と同じロジック）
    bool result = other->CheckCollision(this, info);

    if (result)
    {
        // 法線を反転（AABBから見た法線にする）
        info.contactNormal = -info.contactNormal;

        // otherを自分に設定し直す
        info.other = other->GetOwner();
        info.otherCollider = other;
    }

    return result;
}

// ===================================================================
// デバッグ描画
// ===================================================================
#ifdef DEBUG
void AABBCollider::DebugDraw()
{
    // デバッグ用のボックス描画（後で実装）
    // 現在は何もしない
    // TODO: ワイヤーフレームボックスを描画する関数を実装

    /*
    Vector3 min = GetMin();
    Vector3 max = GetMax();
    Color color = m_IsTrigger ? Color(1, 1, 0, 1) : Color(0, 1, 0, 1);
    DrawWireBox(min, max, color);
    */
}
#endif