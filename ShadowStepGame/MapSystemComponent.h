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

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    MapSystemComponent(const std::string fileName)
    {
        m_DataFile = "data/" + fileName;
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