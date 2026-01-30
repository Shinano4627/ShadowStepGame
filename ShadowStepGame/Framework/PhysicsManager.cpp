// ===================================================================
// PhysicsManager.cpp
// 物理演算・衝突判定マネージャーの実装
// ===================================================================
#include "PhysicsManager.h"
#include "Collider.h"
#include "GameObject.h"
#include <algorithm>
#include <iostream>

// ===================================================================
// 初期化
// ===================================================================
void PhysicsManager::Init()
{
    std::cout << "[PhysicsManager] Initializing..." << std::endl;

    // コライダーリストをクリア
    m_Colliders.clear();
    m_PreviousCollisions.clear();

    // デフォルトの重力設定（Y軸負方向）
    m_Gravity = Vector3(0.0f, -9.8f, 0.0f);

    // レイヤー衝突マトリクスを初期化（すべて有効）
    for (int i = 0; i < Physics::MAX_LAYERS; i++)
    {
        for (int j = 0; j < Physics::MAX_LAYERS; j++)
        {
            m_LayerCollisionMatrix[i][j] = true;
        }
    }

#ifdef DEBUG
    m_DebugDraw = false;
#endif

    std::cout << "[PhysicsManager] Initialized" << std::endl;
}

// ===================================================================
// 終了処理
// ===================================================================
void PhysicsManager::UnInit()
{
    std::cout << "[PhysicsManager] Uninitializing..." << std::endl;

    // コライダーリストをクリア
    // （実際のColliderオブジェクトはGameObjectが管理しているので削除しない）
    m_Colliders.clear();
    m_PreviousCollisions.clear();

    std::cout << "[PhysicsManager] Uninitialized" << std::endl;
}

// ===================================================================
// 更新処理（メインループ）
// ===================================================================
void PhysicsManager::Update()
{
    // ① 無効なコライダーを除外
    CleanupInvalidColliders();

    // ② 衝突判定を実行
    CheckCollisions();

    // ③ デバッグ描画（デバッグビルド時のみ）
#ifdef DEBUG
    if (m_DebugDraw)
    {
        DrawDebug();
    }
#endif
}

// ===================================================================
// Collider登録
// ===================================================================
void PhysicsManager::RegisterCollider(Collider* collider)
{
    if (!collider)
    {
        return;
    }

    // 既に登録されているかチェック
    auto it = std::find(m_Colliders.begin(), m_Colliders.end(), collider);
    if (it == m_Colliders.end())
    {
        m_Colliders.push_back(collider);

#ifdef DEBUG
        std::cout << "[PhysicsManager] Collider registered. Total: "
            << m_Colliders.size() << std::endl;
#endif
    }
}

// ===================================================================
// Collider解除
// ===================================================================
void PhysicsManager::UnregisterCollider(Collider* collider)
{
    if (!collider)
    {
        return;
    }

    // リストから削除
    auto it = std::find(m_Colliders.begin(), m_Colliders.end(), collider);
    if (it != m_Colliders.end())
    {
        m_Colliders.erase(it);

#ifdef DEBUG
        std::cout << "[PhysicsManager] Collider unregistered. Total: "
            << m_Colliders.size() << std::endl;
#endif
    }
}

// ===================================================================
// レイヤー衝突設定
// ===================================================================
void PhysicsManager::SetLayerCollision(int layer1, int layer2, bool enable)
{
    // 範囲チェック
    if (layer1 < 0 || layer1 >= Physics::MAX_LAYERS ||
        layer2 < 0 || layer2 >= Physics::MAX_LAYERS)
    {
        std::cerr << "[PhysicsManager] Invalid layer index: "
            << layer1 << ", " << layer2 << std::endl;
        return;
    }

    // 対称行列として設定
    m_LayerCollisionMatrix[layer1][layer2] = enable;
    m_LayerCollisionMatrix[layer2][layer1] = enable;
}

// ===================================================================
// レイヤー衝突取得
// ===================================================================
bool PhysicsManager::GetLayerCollision(int layer1, int layer2) const
{
    // 範囲チェック
    if (layer1 < 0 || layer1 >= Physics::MAX_LAYERS ||
        layer2 < 0 || layer2 >= Physics::MAX_LAYERS)
    {
        return false;
    }

    return m_LayerCollisionMatrix[layer1][layer2];
}

// ===================================================================
// 衝突判定のメインループ
// ===================================================================
void PhysicsManager::CheckCollisions()
{
    // 今フレームの衝突を記録
    std::unordered_set<CollisionPair, CollisionPair::Hash> currentCollisions;

    // 全コライダーペアをチェック
    for (size_t i = 0; i < m_Colliders.size(); i++)
    {
        for (size_t j = i + 1; j < m_Colliders.size(); j++)
        {
            Collider* col1 = m_Colliders[i];
            Collider* col2 = m_Colliders[j];

            // 衝突判定すべきかチェック
            if (!ShouldCollide(col1, col2))
            {
                continue;
            }

            // 衝突判定実行
            CollisionInfo info;
            if (col1->CheckCollision(col2, info))
            {
                // 衝突ペアを記録
                CollisionPair pair(col1, col2);
                currentCollisions.insert(pair);

                // 前フレームも衝突していたかチェック
                if (m_PreviousCollisions.find(pair) != m_PreviousCollisions.end())
                {
                    // 継続衝突 → OnCollisionStay
                    info.other = col2->GetOwner();
                    info.otherCollider = col2;
                    col1->OnCollisionStay(info);

                    info.other = col1->GetOwner();
                    info.otherCollider = col1;
                    col2->OnCollisionStay(info);
                }
                else
                {
                    // 新規衝突 → OnCollisionEnter
                    info.other = col2->GetOwner();
                    info.otherCollider = col2;
                    col1->OnCollisionEnter(info);

                    info.other = col1->GetOwner();
                    info.otherCollider = col1;
                    col2->OnCollisionEnter(info);
                }
            }
        }
    }

    // OnCollisionExit 処理
    // （前フレームにあって今フレームにない衝突）
    for (const auto& pair : m_PreviousCollisions)
    {
        if (currentCollisions.find(pair) == currentCollisions.end())
        {
            // 衝突が終了した
            CollisionInfo info;

            info.other = pair.collider2->GetOwner();
            info.otherCollider = pair.collider2;
            pair.collider1->OnCollisionExit(info);

            info.other = pair.collider1->GetOwner();
            info.otherCollider = pair.collider1;
            pair.collider2->OnCollisionExit(info);
        }
    }

    // 前フレームの衝突を更新
    m_PreviousCollisions = std::move(currentCollisions);
}

// ===================================================================
// 2つのコライダーが衝突判定すべきかチェック
// ===================================================================
bool PhysicsManager::ShouldCollide(Collider* col1, Collider* col2) const
{
    // nullチェック
    if (!col1 || !col2)
    {
        return false;
    }

    // 有効性チェック
    if (!col1->IsEnabled() || !col2->IsEnabled())
    {
        return false;
    }

    // オーナーの有効性チェック
    GameObject* owner1 = col1->GetOwner();
    GameObject* owner2 = col2->GetOwner();

    if (!owner1 || !owner2)
    {
        return false;
    }

    if (!owner1->IsActive() || !owner2->IsActive())
    {
        return false;
    }

    // 静的オブジェクト同士はスキップ（最適化）
    if (col1->IsStatic() && col2->IsStatic())
    {
        return false;
    }

    // レイヤーマスクチェック
    if (!GetLayerCollision(col1->GetLayer(), col2->GetLayer()))
    {
        return false;
    }

    return true;
}

// ===================================================================
// 無効なコライダーを削除
// ===================================================================
void PhysicsManager::CleanupInvalidColliders()
{
    m_Colliders.erase(
        std::remove_if(m_Colliders.begin(), m_Colliders.end(),
            [](Collider* col)
            {
                return col == nullptr || col->GetOwner() == nullptr;
            }),
        m_Colliders.end()
    );
}

// ===================================================================
// デバッグ描画
// ===================================================================
#ifdef DEBUG
void PhysicsManager::DrawDebug()
{
    for (auto collider : m_Colliders)
    {
        if (collider && collider->IsEnabled())
        {
            collider->DebugDraw();
        }
    }
}
#endif