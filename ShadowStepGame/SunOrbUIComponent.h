// ===================================================================
// SunOrbUIComponent.h
// 太陽の位置をUI上に弧状で表示するコンポーネント
// ===================================================================
#pragma once
#include "Framework/Component.h"
#include "Framework/GameObject.h"
#include "SunManageComponent.h"
#include <cmath>

class SunOrbUIComponent : public Component
{
private:
    SunManageComponent* m_pSunSystem = nullptr;  // 太陽システムへの参照
    
    // 弧の設定
    DirectX::SimpleMath::Vector3 m_CenterPos;    // 弧の中心位置
    float m_Radius = 50.f;                        // 弧の半径

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SunOrbUIComponent(const DirectX::SimpleMath::Vector3& centerPos, float radius)
        : m_CenterPos(centerPos)
        , m_Radius(radius)
    {
    }

    // ===================================================================
    // 初期化処理
    // ===================================================================
    void Init() override;

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override;

    // ===================================================================
    // SunSystemの設定
    // ===================================================================
    void SetSunSystem(SunManageComponent* sunSystem)
    {
        m_pSunSystem = sunSystem;
    }

private:
    // ===================================================================
    // 弧上の位置を計算
    // ===================================================================
    DirectX::SimpleMath::Vector3 CalcArcPosition(float progress);
};
