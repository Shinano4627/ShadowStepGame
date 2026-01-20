#pragma once
#include "Component.h"
#include <vector>
// ===================================================================
// ユニットコンポネント
// ユニット一体分の状態・ターン管理
// ===================================================================

class UnitComponent :public Component
{
public:
    //陣営
    enum class UnitCamp
    {
        UnitPlayer, //プレイヤー
        UnitEnemy,  //エネミー
    };

    //状態
    enum class UnitState
    {
        UnitWait,   //待機
        UnitActive, //行動中
        UnitDown,   //ダウン
        UnitDelete, //消滅
    };

    //種類
    enum UnitModel
    {
        UnitNormal,       //通常
        UnitAttacker,     //攻撃
        UnitPlacementer,  //配置
        UnitGiant,        //巨人
    };

    //マップ座標
    struct MapPosition
    {
        int posX;
        int posZ;
    };

    //ユニット情報
    struct UnitStatus
    {
        int id = -1;        //オブジェクトID
        MapPosition pos;    //マップ上の位置
        int hp = 5;         //HP
        int speed = 5;      //素早さ
        std::vector<MapPosition> shadowPosList; //影範囲
        UnitCamp camp;      //陣営
        UnitState state;    //状態
        UnitModel model;    //種類
    };
private:
    UnitStatus m_status;

    MapPosition m_gridPos;      //現在の座標
    bool m_hasMoved = false;    //このターンで移動済みか

public:
 
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    UnitComponent();
    // ===================================================================
    // デストラクタ
    // ===================================================================
    ~UnitComponent(){}

    // ===================================================================
    // ターン管理
    // ===================================================================
    void BeginTurn();               //ターン開始
    void EndTurn();                 //ターン終了
    bool HasFinishedTurn() const;   //ターン終了判定
    

    bool IsMyTurn() const{return m_status.state == UnitState::UnitActive;}
    bool IsActive() const{return m_status.state == UnitState::UnitActive;}
    bool CanAct() const;            //行動可能か


    // ===================================================================
    // 更新
    // ===================================================================
    void Update();

    // Getter / Setter
    UnitCamp GetCamp() const { return m_status.camp; }
    UnitState GetState() const { return m_status.state; }
    int GetHP() const { return m_status.hp; }
    void SetCamp(UnitCamp camp) { m_status.camp = camp; }

    const MapPosition& GetGridPos() const { return m_gridPos; }
    void SetGridPos(const MapPosition& pos) { m_gridPos = pos; }
    bool HasMoved() const { return m_hasMoved; }
    void SetMoved(bool moved) { m_hasMoved = moved; }
};
