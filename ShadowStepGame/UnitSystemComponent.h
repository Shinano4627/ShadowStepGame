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
    enum UnitState
    {
        UnitWait,
        UnitActive,
        UnitDown,
        UnitDelete,
    };

    enum UnitType
    {
        UnitPlayer,
        UnitEnemy,
    };

    enum UnitModel
    {
        UnitShort,  // 近距離攻撃タイプ
        UnitLong,   // 遠距離攻撃タイプ
    };

    struct MapPosision
    {
        int posX;   
        int posZ;   
    };

    struct UnitStatus
    {
        // サイズはトランスフォームを使用する
        int id; // オブジェクトリストと一致させる
        MapPosision pos;
        int hp;
        int speed;
        bool isBig; // 巨大化状態か
        std::vector<MapPosision> shadowPosList;
        UnitType type;
        UnitState state;      // 状態
    };

private:
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
    void Update() override
    {
        if (!m_pOwner) return;

        // ステータスが削除状態のユニットを削除する
    }

    // ===================================================================
    // 終了処理
    // ===================================================================
    void Uninit() override
    {
        if (!m_pOwner) return;

        m_UnitList.clear();

        // 光の方向を取得して影を生成する
    }

    // ===================================================================
    // 独自処理
    // ===================================================================
    void MakeUnit(UnitModel model, std::vector<std::unique_ptr<GameObject>>& objectList);      // 指定したタイプのユニット作成
    void DeleteUnits(std::vector<std::unique_ptr<GameObject>>& objectList);      // 削除状態のユニットをすべて削除する
    void GetAllUnits();  // 全ユニットデータ取得
    void GetUnit(int id); // idを指定してユニットデータ取得
    void GetSurviveUnits(); // 生存かつダウンしていないユニットすべてを速さ順にソートして取得

    void SetUnitActive(int id); // idを指定してユニットをActiveに

    // 影の範囲内かを判定する

    // ===================================================================
    // 設定
    // ===================================================================
    //void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    //float GetMoveSpeed() const { return m_MoveSpeed; }
};