// ===================================================================
// UnitSystemComponent.h
// ユニットの集合管理
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include "UnitCommon.h"
#include "UnitComponent.h"

class UnitSystemComponent : public Component
{

private:
    // UnitComponentList
    std::vector<UnitComponent*> m_UnitList;

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    UnitSystemComponent()
    {
        m_UnitList.clear(); // リスト初期化
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
    void Update() override;

    // ===================================================================
    // 終了処理
    // ===================================================================
    void Uninit() override;

    //=======================================
    // 登録・削除
    //=======================================
    void RegisterUnit(UnitComponent* unit);
    void UnRegisterUnit(UnitComponent* unit);

    //=======================================
    // 取得関数
    //=======================================
    const std::vector<UnitComponent*> GetAllUnits() const;
    std::vector<UnitComponent*> GetAliveUnits() const;

    // Player ユニット取得
    std::vector<UnitComponent*> GetPlayerUnits() const;
    // Enemy ユニット取得（今後用・対称性）
    std::vector<UnitComponent*> GetEnemyUnits() const;
    
    // -------- ターン関連 --------
    // 速度順並べ替えList取得
    std::vector<UnitComponent*> GetUnitsSortedBySpeed() const;

    //=======================================
    // 検索
    //=======================================
    UnitComponent* FindUnit(int id);
    UnitComponent* FindUnitAtPosition(const MapPosition& pos) const;

    //=======================================
    // 勝敗判定
    //=======================================
    bool IsPlayerAllDead() const;
    bool IsEnemyAllDead() const;

};
