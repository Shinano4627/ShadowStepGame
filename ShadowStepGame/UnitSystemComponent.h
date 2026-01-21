// ===================================================================
// UnitSystemComponent.h
// ユニットの制御を行う
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include "UnitCommon.h"


class UnitSystemComponent : public Component
{

private:
    // UnitStatusList
    std::vector<std::unique_ptr<UnitStatus>> m_UnitList;

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
    void Update() override;

    // ===================================================================
    // 終了処理
    // ===================================================================
    void Uninit() override;

    //=======================================
    // 登録・削除
    //=======================================
    void RegisterUnit(const UnitStatus& status);
    void KillUnit(int id);      // Listから削除
    void DamageUnit(int id, int damage);    // 0以下ならKill     

    //=======================================
    // 取得関数
    //=======================================
    std::vector<UnitStatus*> GetAllUnits() const;
    std::vector<UnitStatus*> GetUnitsSortedBySpeed() const;

    //=======================================
    // 検索
    //=======================================
    UnitStatus* FindUnit(int id);

    //=======================================
    // 勝敗判定
    //=======================================
    bool IsPlayerAllDead() const;
    bool IsEnemyAllDead() const;

};