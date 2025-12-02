// ===================================================================
// PhysicsManager.h
// 物理演算・衝突判定を一元管理するマネージャークラス
// ===================================================================
#pragma once
#include "singleton.h"
#include "PhysicsCommon.h"
#include <vector>
#include <unordered_set>

// 前方宣言
class Collider;

// ===================================================================
// PhysicsManager クラス
// 全Colliderの管理と衝突判定のメインループを担当
// ===================================================================
class PhysicsManager
{
private:
    // ===================================================================
    // メンバ変数
    // ===================================================================

    // 登録されたすべてのコライダー
    std::vector<Collider*> m_Colliders;

    // 前フレームの衝突ペア（OnCollisionExitの検出用）
    std::unordered_set<CollisionPair, CollisionPair::Hash> m_PreviousCollisions;

    // レイヤー間の衝突マトリクス（32x32）
    bool m_LayerCollisionMatrix[Physics::MAX_LAYERS][Physics::MAX_LAYERS];

    // 重力加速度
    Vector3 m_Gravity;

    // デバッグ描画フラグ
#ifdef DEBUG
    bool m_DebugDraw;
#endif

public:
    // ===================================================================
    // ライフサイクル
    // ===================================================================
    void Init();
    void UnInit();
    void Update();

    // ===================================================================
    // Collider登録・解除
    // ===================================================================
    void RegisterCollider(Collider* collider);
    void UnregisterCollider(Collider* collider);

    // ===================================================================
    // レイヤー衝突設定
    // ===================================================================
    void SetLayerCollision(int layer1, int layer2, bool enable);
    bool GetLayerCollision(int layer1, int layer2) const;

    // ===================================================================
    // 重力設定
    // ===================================================================
    void SetGravity(const Vector3& gravity) { m_Gravity = gravity; }
    const Vector3& GetGravity() const { return m_Gravity; }

    // ===================================================================
    // デバッグ機能
    // ===================================================================
#ifdef DEBUG
    void SetDebugDraw(bool enable) { m_DebugDraw = enable; }
    bool IsDebugDraw() const { return m_DebugDraw; }
    void DrawDebug();
#endif

private:
    // ===================================================================
    // 内部処理
    // ===================================================================

    // 衝突判定のメインループ
    void CheckCollisions();

    // 2つのコライダーが衝突判定すべきかチェック
    bool ShouldCollide(Collider* col1, Collider* col2) const;

    // 無効なコライダーを削除
    void CleanupInvalidColliders();
};

// ===================================================================
// シングルトンアクセス用マクロ
// ===================================================================
#define PHYSICS_MANAGER Singleton<PhysicsManager>::GetInstance()