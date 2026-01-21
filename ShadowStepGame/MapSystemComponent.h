// ===================================================================
// MapSystemComponent.h
// マップの制御を行う
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include "UnitCommon.h"

class MapSystemComponent : public Component
{
private:
    std::string m_DataFile;      // CSVファイル名
    int m_MapWidth = 0;     // CSVから読み取り
    int m_MapHeight = 0;    // CSVから読み取り
    int** m_MapData = nullptr;  // CSVから読み取ったデータを数値で管理

    float m_SizePiece = 5.f;
    float m_DrawStartPosX = 0.f;
    float m_DrawStartPosZ = 0.f;

    enum class EMapTile
    {
        Empty = 0,  // 何もない
        Wall = 1,   // 壁
        Player = 2, // プレイヤー
        Enemy = 3,  // 敵
        Tree = 4,   // 樹
        Shadow = 5, // 影
    };

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    MapSystemComponent(const std::string fileName)
    {
        m_DataFile = "data/" + fileName;
    }

    // ===================================================================
    // デストラクタ
    // ===================================================================
    ~MapSystemComponent()
    {
        // マップ
        for (int i = 0; i < m_MapHeight; i++)
        {
            delete[] m_MapData[i];
        }
        delete[] m_MapData;
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_pOwner) return;
    }

    // ===================================================================
    // GameSystemで行うMap更新処理
    // UnitData,ShadowData,地形MapDataを元にMapDataを更新する
    // ===================================================================
    void UpdateMap(const std::vector<UnitStatus*>& units,
        const int* const* shadowMap);

    void MakeMap(std::vector<std::unique_ptr<GameObject>>& objectList);      // CSVデータ読み込みとマップオブジェクトの作成

    // ===================================================================
    // 設定
    // ===================================================================
    float GetMapSizeHeight() const { return (float)m_MapHeight * m_SizePiece; }
    float GetMapSizeWidth() const { return (float)m_MapWidth * m_SizePiece; }
    int GetMapHeight() const { return m_MapHeight; }
    int GetMapWidth() const { return m_MapWidth; }
    const int* const* GetRawMapData() const;
};