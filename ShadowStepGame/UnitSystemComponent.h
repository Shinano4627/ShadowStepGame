// ===================================================================
// UnitSystemComponent.h
// ユニットの制御を行う
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"

class UnitSystemComponent : public Component
{
private:

    struct UnitInfo
    {
        // 敵かプレイヤー
        // 位置情報
    };

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    UnitSystemComponent()
    {
    }

    // ===================================================================
    // デストラクタ
    // ===================================================================
    ~UnitSystemComponent()
    {
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_pOwner) return;
    }

    void MakeMap(std::vector<std::unique_ptr<GameObject>>& objectList);      // CSVデータ読み込みとマップオブジェクトの作成

    // Unitの
    std::vector<UnitInfo> GetUnitInfo();

    // ===================================================================
    // 設定
    // ===================================================================
    //void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    //float GetMoveSpeed() const { return m_MoveSpeed; }
};