// ===================================================================
// SunOrbUIComponent.cpp
// 太陽の位置をUI上に弧状で表示するコンポーネント
// ===================================================================
#include "SunOrbUIComponent.h"

using namespace DirectX::SimpleMath;

// ===================================================================
// 初期化処理
// ===================================================================
void SunOrbUIComponent::Init()
{
    if (!m_pOwner) return;

    // 初期位置を設定
    Vector3 initPos = CalcArcPosition(0.f);
    m_pOwner->GetTransform().SetPosition(initPos);
}

// ===================================================================
// 更新処理
// ===================================================================
void SunOrbUIComponent::Update()
{
    if (!m_pOwner || !m_pSunSystem) return;

    // 現在の進捗を取得
    int currentTurn = m_pSunSystem->GetTurnProgress();
    int maxTurn = m_pSunSystem->GetMaxTurn();

    // 進捗率を計算（0.0 ～ 1.0）
    float progress = 0.f;
    if (maxTurn > 0)
    {
        progress = static_cast<float>(currentTurn) / static_cast<float>(maxTurn);
    }

    // 弧上の位置を計算して設定
    Vector3 newPos = CalcArcPosition(progress);
    m_pOwner->GetTransform().SetPosition(newPos);
}

// ===================================================================
// 弧上の位置を計算
// progress: 0.0（左端）～ 1.0（右端）
// ===================================================================
Vector3 SunOrbUIComponent::CalcArcPosition(float progress)
{
    // 角度を計算（πからスタートして0へ）
    // 左端(progress=0) → 角度π（180度）
    // 頂点(progress=0.5) → 角度π/2（90度）
    // 右端(progress=1) → 角度0（0度）
    float angle = DirectX::XM_PI * (1.f - progress);

    // 弧上の位置を計算
    float x = m_CenterPos.x + m_Radius * cosf(angle);
    float y = m_CenterPos.y + m_Radius * sinf(angle);

    return Vector3(x, y, 0.f);
}
