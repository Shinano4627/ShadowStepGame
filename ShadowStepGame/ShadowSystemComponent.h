// ===================================================================
// ShadowSystemComponent.h
// すべての影を管理
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include "ShadowStepGameCommon.h"

class ShadowSystemComponent : public Component
{
private:
    int** m_ShadowMapData = nullptr; 

    int m_MapWidth = 0;   // 初期設定から変更しない想定
    int m_MapHeight = 0;  

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    ShadowSystemComponent(int mapWidth, int mapHeight)
        : m_MapWidth(mapWidth)
        , m_MapHeight(mapHeight)
    {
        MakeShadowMap();
    }

    // ===================================================================
    // デストラクタ
    // ===================================================================
    ~ShadowSystemComponent()
    {
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_pOwner) return;
    }

    // ===================================================================
    // 内部関数
    // ===================================================================
    void MakeShadowMap()
    {
        // 空データの作成
    }

    void UpdateShadowMap(const int* mapData, const CellPosision* sunPos);   // 光源情報を元に全てのオブジェクトの影を更新する

    // ===================================================================
    // ゲッター・セッター
    // ===================================================================
    const int** GetShdowMap() { return m_ShadowMapData;};
};
