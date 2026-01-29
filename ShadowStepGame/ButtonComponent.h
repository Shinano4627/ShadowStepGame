// ===================================================================
// ButtonComponent.h
// ボタンコンポーネント
// 選択状態の管理とカーソル判定を行う
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Texture2D.h"
#include "CursorManager.h"

// ===================================================================
// ButtonComponent クラス
// ===================================================================
class ButtonComponent : public Component
{
public:

private:
    // ===================================================================
    // メンバ変数
    // ===================================================================
    bool m_SelectedPre = false;     // 前回の選択状態
    bool m_IsSelected = false;      // 選択状態
    DirectX::SimpleMath::Vector3 m_RangeHitScale;
public:
    // ===================================================================
    // コンストラクタ・デストラクタ
    // ===================================================================
    ButtonComponent(const DirectX::SimpleMath::Vector3& rangeHitScale)
        : m_RangeHitScale(rangeHitScale)
    {}
    ~ButtonComponent() override = default;

    // ===================================================================
    // ライフサイクル
    // ===================================================================
    void Init() override;
    void Update() override;
    void Uninit() override;

    // ===================================================================
    // 選択状態
    // ===================================================================
    bool IsSelected() const { return m_IsSelected; }
    void SetSelected(bool selected);

private:
    // ===================================================================
    // 内部処理
    // ===================================================================
    // カーソルとの当たり判定
    bool CheckCursorHit() const;

    // UV切り替え
    void UpdateUV();
};
