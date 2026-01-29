//=======================================
// ShadowSystemComponent.h
// すべての影を管理
//=======================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "GameObjectList.h"
#include "IOManager.h"
#include "Game.h"
#include "UnitCommon.h"
#include <algorithm>
#include <vector>

// コンポーネント
#include "SimplePlaneRendererComponent.h"
#include "Texture2D.h"

struct ShadowParam
{
    MapPosition lightDirection = {0,0};
    int length;    // 影の長さ
};

class ShadowSystemComponent : public Component
{
private:
    // 影マップ（0:影なし,1:影あり）
    int** m_ShadowMapData = nullptr; 
    // 影の元オブジェクトIDマップ（0:なし）
    int** m_ShadowSourceMap = nullptr;

    int m_MapWidth = 0;
    int m_MapHeight = 0;

    // 描画用パラメータ
    float m_SizePiece = 0.0f;
    float m_DrawStartPosX = 0.0f;
    float m_DrawStartPosZ = 0.0f;

    // 影オブジェクトの高さ（マップタイルより少し上）
    const float m_ShadowHeight = 0.5f;

    // 影のテクスチャ（xmlから取得）
    std::string m_ShadowTexturePath = "";

    // 影オブジェクト用ID 開始値5000
    int m_LastObjectId = 5000;

public:
    //=======================================
    // コンストラクタ
    //=======================================
    ShadowSystemComponent(){}

    //=======================================
    // デストラクタ
    //=======================================
    ~ShadowSystemComponent()
    {
        for (int z = 0; z < m_MapHeight; ++z)
        {
            delete[] m_ShadowMapData[z];
        }
        delete[] m_ShadowMapData;
    }

    //=======================================
    // 初期化
    //=======================================
    void Init(){}
    void SetUp(int mapWidth, int mapHeight, float sizePiece, float startPosX, float startPosZ,
               const int* const* mapData, GameObjectList* gameObjectList)
    {
        m_MapWidth = mapWidth;
        m_MapHeight = mapHeight;
        m_SizePiece = sizePiece;
        m_DrawStartPosX = startPosX;
        m_DrawStartPosZ = startPosZ;

        MakeShadowMap();
        CreateShadowObjects(mapData, gameObjectList);
    }

    //=======================================
    // 更新
    //=======================================
    void Update() override {}

    //=======================================
    // 影計算
    //=======================================
    void UpdateShadowMap(
        const int* const* mapData,
        const ShadowParam& param,
        GameObjectList* gameObjectList);

    //=======================================
    // 影の大きさ・向き
    //=======================================
    ShadowParam CalcShadowParm(
        MapPosition lightDirection,
        int mapWidth, int mapHeight);

    //=======================================
    // 取得
    //=======================================
    const int* const* GetShadowMap() const
    { return m_ShadowMapData;};
    // 影の元IDを取得
    int GetShadowSourceAt(int mapX, int mapZ) const
    {
        if (mapX < 0 || mapX >= m_MapWidth || mapZ < 0 || mapZ >= m_MapHeight) return 0;
        return m_ShadowSourceMap[mapZ][mapX];
    }

private:
    //=======================================
    // 影マップ生成
    //=======================================
    void MakeShadowMap();

    //=======================================
    // 初期化
    //=======================================
    void ClearShadowMap();

    //=======================================
    // 影オブジェクト生成（MapDataが0以外のセルのみ）
    //=======================================
    void CreateShadowObjects(const int* const* mapData, GameObjectList* gameObjectList);

    //=======================================
    // 影オブジェクトの表示/Scale/Positionを更新
    //=======================================
    void UpdateShadowObjects(const int* const* mapData, const ShadowParam& param, GameObjectList* gameObjectList);
};
