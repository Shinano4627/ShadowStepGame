// ===================================================================
// ButtonComponent.cpp
// ボタンコンポーネント実装
// ===================================================================
#include "ButtonComponent.h"

using namespace DirectX::SimpleMath;

// ===================================================================
// 初期化処理
// ===================================================================
void ButtonComponent::Init()
{
    m_IsSelected = false;
    m_SelectedPre = false;

    // 初期UV設定（非選択状態）
    UpdateUV();
}

// ===================================================================
// 更新処理
// ===================================================================
void ButtonComponent::Update()
{
    // カーソルとの当たり判定
    bool hit = CheckCursorHit();

    if (hit)
    {
        m_IsSelected = true;
    }

    // 選択状態が変化した場合のみUV更新
    if (m_IsSelected != m_SelectedPre)
    {
        m_SelectedPre = m_IsSelected;
        UpdateUV();
    }
}

// ===================================================================
// 終了処理
// ===================================================================
void ButtonComponent::Uninit()
{
    m_IsSelected = false;
}

// ===================================================================
// 選択状態設定
// ===================================================================
void ButtonComponent::SetSelected(bool selected)
{
    m_IsSelected = selected;
}

// ===================================================================
// カーソルとの当たり判定
// ===================================================================
bool ButtonComponent::CheckCursorHit() const
{
    if (!m_pOwner) return false;

    // カーソル位置を取得
    Vector2 cursorPos = CURSOR_MANAGER.GetPoint();

    // ボタンの位置とサイズを取得
    Transform& transform = m_pOwner->GetTransform();
    Vector3 pos = transform.GetPosition();

    // 矩形範囲を計算（中心がPosition、Scaleが幅と高さ）
    float left = pos.x - m_RangeHitScale.x / 2.f;
    float right = pos.x + m_RangeHitScale.x / 2.f;
    float top = pos.y + m_RangeHitScale.y / 2.f;
    float bottom = pos.y - m_RangeHitScale.y / 2.f;

    // カーソルが矩形内にあるか判定
    if (cursorPos.x >= left && cursorPos.x <= right &&
        cursorPos.y >= bottom && cursorPos.y <= top)
    {
        return true;
    }

    return false;
}

// ===================================================================
// UV切り替え
// ===================================================================
void ButtonComponent::UpdateUV()
{
    if (!m_pOwner) return;

    // Texture2Dコンポーネントを取得
    Texture2D* tex = m_pOwner->GetMeshComponent<Texture2D>();
    if (!tex) return;

    // UV設定（横2分割）
    // 非選択: nu=1 → u=0~0.5
    // 選択:   nu=2 → u=0.5~1
    if (m_IsSelected)
    {
        tex->SetUV(2, 1, 2, 1);
    }
    else
    {
        tex->SetUV(1, 1, 2, 1);
    }
}
