// ===================================================================
// Collider.h
// 衝突判定コンポーネントの基底クラス
// ===================================================================
#pragma once
#include "Component.h"
#include "PhysicsCommon.h"
#include <functional>

// ===================================================================
// Collider 基底クラス
// すべてのコライダーはこのクラスを継承する
// ===================================================================
class Collider : public Component
{
protected:
    // ===================================================================
    // メンバ変数
    // ===================================================================
    ColliderType m_Type;             // コライダーの種類
    Vector3 m_Center;                // ローカル座標系での中心
    Vector3 m_Offset;                // オフセット
    bool m_IsTrigger;                // トリガーモード（物理挙動なし）
    bool m_IsStatic;                 // 静的オブジェクト（動かない）
    int m_Layer;                     // レイヤー（フィルタリング用）

    // コールバック関数
    CollisionCallback m_OnCollisionEnter;  // 衝突開始時
    CollisionCallback m_OnCollisionStay;   // 衝突中
    CollisionCallback m_OnCollisionExit;   // 衝突終了時

public:
    // ===================================================================
    // コンストラクタ・デストラクタ
    // ===================================================================
    Collider();
    virtual ~Collider() = default;

    // ===================================================================
    // ライフサイクル
    // ===================================================================
    void Init() override;
    void Uninit() override;

    // ===================================================================
    // 衝突判定（純粋仮想関数）
    // 派生クラスで実装必須
    // ===================================================================
    virtual bool CheckCollision(Collider* other, CollisionInfo& info) = 0;

    // ===================================================================
    // ワールド座標取得
    // ===================================================================
    Vector3 GetWorldCenter() const;

    // ===================================================================
    // プロパティ設定・取得
    // ===================================================================

    // トリガーモード
    void SetTrigger(bool trigger) { m_IsTrigger = trigger; }
    bool IsTrigger() const { return m_IsTrigger; }

    // 静的オブジェクト
    void SetStatic(bool isStatic) { m_IsStatic = isStatic; }
    bool IsStatic() const { return m_IsStatic; }

    // レイヤー
    void SetLayer(int layer) { m_Layer = layer; }
    int GetLayer() const { return m_Layer; }

    // コライダータイプ
    ColliderType GetType() const { return m_Type; }

    // 中心・オフセット
    void SetCenter(const Vector3& center) { m_Center = center; }
    const Vector3& GetCenter() const { return m_Center; }

    void SetOffset(const Vector3& offset) { m_Offset = offset; }
    const Vector3& GetOffset() const { return m_Offset; }

    // ===================================================================
    // コールバック設定
    // ===================================================================
    void SetOnCollisionEnter(CollisionCallback callback) { m_OnCollisionEnter = callback; }
    void SetOnCollisionStay(CollisionCallback callback) { m_OnCollisionStay = callback; }
    void SetOnCollisionExit(CollisionCallback callback) { m_OnCollisionExit = callback; }

    // ===================================================================
    // 衝突イベント発火（PhysicsManagerから呼ばれる）
    // ===================================================================
    void OnCollisionEnter(const CollisionInfo& info);
    void OnCollisionStay(const CollisionInfo& info);
    void OnCollisionExit(const CollisionInfo& info);

    // ===================================================================
    // デバッグ描画（デバッグビルド時のみ）
    // ===================================================================
#ifdef DEBUG
    virtual void DebugDraw() = 0;
#endif
};