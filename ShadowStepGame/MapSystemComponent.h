// ===================================================================
// MapSystemComponent.h
// マップの制御を行う
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"

class MapSystemComponent : public Component
{
private:
    std::string m_DataFile;      // CSVファイル名
    int m_MapWidth;     // CSVから読み取り
    int m_MapHeight;    // CSVから読み取り
    int** m_MapData;  // CSVから読み取ったデータを数値で管理

    float m_SizePiece = 5.f;
    float m_DrawStartPosX = 0.f;
    float m_DrawStartPosZ = 0.f;

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

    void MakeMap(std::vector<std::unique_ptr<GameObject>>& objectList);      // CSVデータ読み込みとマップオブジェクトの作成

    // ===================================================================
    // 設定
    // ===================================================================
    //void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    //float GetMoveSpeed() const { return m_MoveSpeed; }
};