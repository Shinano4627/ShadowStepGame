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
#include "UnitComponent.h"

// 前方宣言
class GameObjectList;

enum class SMapTile
{
    Empty = 0,   // 行動範囲外
    Move = 1,
    Attack = 2,
    Place = 3
};

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
    float m_DrawStartPosY = 0.2f;

    // SelectMap用
    int** m_SelectMapData = nullptr;  //

    // SelectMap 用 GameObject（全マス分）
    GameObject*** m_SelectMapObjects = nullptr;
    // 選択カーソル GameObject
    GameObject* m_pSelectCursor = nullptr;

    // SelectMap が有効かどうか
    bool m_IsSelectMapActive = false;
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

        if (m_SelectMapData)
        {
            for (int z = 0; z < m_MapHeight; z++)
                delete[] m_SelectMapData[z];
            delete[] m_SelectMapData;
        }

        if (m_SelectMapObjects)
        {
            for (int z = 0; z < m_MapHeight; z++)
                delete[] m_SelectMapObjects[z];
            delete[] m_SelectMapObjects;
        }

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
    void UpdateMap(const std::vector<UnitComponent*>& units,
        const int* const* shadowMap);

    void MakeMap(std::unique_ptr<GameObjectList>& objectList);      // CSVデータ読み込みとマップオブジェクトの作成

    // Unit(中心原点) → Map配列
    bool ConvertUnitPosToMapIndex(
        int unitX, int unitZ,
        int& outMapX, int& outMapZ) const;

    // Map配列 → Unit(中心原点)
    bool ConvertMapIndexToUnitPos(
        int mapX, int mapZ,
        int& outUnitX, int& outUnitZ) const;

    // Unit位置のマス取得
    EMapTile GetTileAtUnitPos(int unitX, int unitZ) const;

    // 移動可能判定
    bool IsWalkableAtUnitPos(int unitX, int unitZ) const;
    // 攻撃可能判定
    bool IsAttackableAtUnitPos(int fromX, int fromZ,
        int toX, int toZ,UnitType type) const;
    // 配置可能判定
    bool IsPlacebleAtUnitPos(int x, int z) const;

    // SelectMap用関数(絶対に通常Mapを作成してから呼ぶこと！)
    void MakeSelectMap(std::unique_ptr<GameObjectList>& objectList);

    void StartSelectMap(UnitComponent* unit,MapPosition selectpos);
    void UpdateSelectCursor(MapPosition selectpos);
    void EndSelectMap();

    // ===================================================================
    // 設定
    // ===================================================================
    float GetMapSizeHeight() const { return (float)m_MapHeight * m_SizePiece; }
    float GetMapSizeWidth() const { return (float)m_MapWidth * m_SizePiece; }
    int GetMapHeight() const { return m_MapHeight; }
    int GetMapWidth() const { return m_MapWidth; }
    const int* const* GetRawMapData() const;
    float GetSizePiece() const { return m_SizePiece; }
    float GetDrawStartPosX() const { return m_DrawStartPosX; }
    float GetDrawStartPosZ() const { return m_DrawStartPosZ; }
    Vector3 GetPositionToMap(MapPosition map) const     // マップのセル位置が描画上のどの位置になるかを取得する
    {
        return Vector3(m_DrawStartPosX + map.x * m_SizePiece, m_DrawStartPosY, m_DrawStartPosZ + map.z * m_SizePiece);
    }    
};