// ===================================================================
// SunOrbUIComponent.h
// 太陽の位置をUI上に楕円弧状で表示するコンポーネント
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
    
    // 楕円弧の設定
    DirectX::SimpleMath::Vector3 m_CenterPos;    // 楕円の中心位置
    float m_RadiusX = 100.f;                      // X方向の半径（横幅）
    float m_RadiusY = 50.f;                       // Y方向の半径（高さ）

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SunOrbUIComponent(const DirectX::SimpleMath::Vector3& centerPos, float radiusX, float radiusY)
        : m_CenterPos(centerPos)
        , m_RadiusX(radiusX)
        , m_RadiusY(radiusY)
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
    // 楕円弧上の位置を計算
    // ===================================================================
    DirectX::SimpleMath::Vector3 CalcArcPosition(float progress);
};
