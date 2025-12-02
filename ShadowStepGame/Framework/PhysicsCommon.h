// ===================================================================
// PhysicsCommon.h
// 物理演算システムの共通定義
// ===================================================================
#pragma once
#include "SystemCommon.h"
#include <functional>
#include <unordered_set>

// 前方宣言
class GameObject;
class Collider;

// ===================================================================
// 物理演算用の定数
// ===================================================================
namespace Physics
{
    constexpr int MAX_LAYERS = 32;              // 最大レイヤー数
    constexpr float COLLISION_EPSILON = 0.0001f; // 衝突判定の誤差許容範囲
}

// ===================================================================
// コライダータイプ列挙型
// ===================================================================
enum class ColliderType
{
    SPHERE,   // 球形
    AABB,     // 軸並行境界ボックス
    OBB,      // 方向付き境界ボックス（将来拡張）
    CAPSULE   // カプセル（将来拡張）
};

// ===================================================================
// 衝突情報構造体
// ===================================================================
struct CollisionInfo
{
    GameObject* other;           // 衝突相手のGameObject
    Collider* otherCollider;     // 衝突相手のCollider

    Vector3 contactPoint;        // 衝突点（ワールド座標）
    Vector3 contactNormal;       // 衝突面の法線ベクトル
    float penetrationDepth;      // めり込み深さ

    Vector3 relativeVelocity;    // 相対速度（将来の物理演算用）
    float timestamp;             // 衝突発生時刻

    // コンストラクタ
    CollisionInfo()
        : other(nullptr)
        , otherCollider(nullptr)
        , contactPoint(Vector3::Zero)
        , contactNormal(Vector3::Zero)
        , penetrationDepth(0.0f)
        , relativeVelocity(Vector3::Zero)
        , timestamp(0.0f)
    {}
};

// ===================================================================
// 衝突ペア（PhysicsManager内部で使用）
// ===================================================================
struct CollisionPair
{
    Collider* collider1;
    Collider* collider2;

    // コンストラクタ
    CollisionPair()
        : collider1(nullptr)
        , collider2(nullptr)
    {}

    CollisionPair(Collider* c1, Collider* c2)
        : collider1(c1)
        , collider2(c2)
    {}

    // 等価比較演算子（順序を考慮しない）
    bool operator==(const CollisionPair& other) const
    {
        return (collider1 == other.collider1 && collider2 == other.collider2) ||
            (collider1 == other.collider2 && collider2 == other.collider1);
    }

    // ハッシュ関数（std::unordered_setで使用）
    struct Hash
    {
        size_t operator()(const CollisionPair& pair) const
        {
            // ポインタのアドレスをハッシュ化
            size_t h1 = std::hash<Collider*>()(pair.collider1);
            size_t h2 = std::hash<Collider*>()(pair.collider2);

            // 順序に依存しないハッシュを生成
            if (pair.collider1 < pair.collider2)
                return h1 ^ (h2 << 1);
            else
                return h2 ^ (h1 << 1);
        }
    };
};

// ===================================================================
// コールバック型定義
// ===================================================================
using CollisionCallback = std::function<void(const CollisionInfo&)>;