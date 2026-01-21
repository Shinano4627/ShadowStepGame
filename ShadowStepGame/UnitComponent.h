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

    ////状態
    //enum class UnitState
    //{
    //    UnitWait,   //待機
    //    UnitActive, //行動中
    //    UnitDown,   //ダウン
    //    UnitDelete, //消滅
    //};

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
        //std::vector<MapPosition> shadowPosList; //影範囲
        UnitCamp camp;      //陣営
        //UnitState state;    //状態
        UnitModel model;    //種類
    };
private:
    UnitStatus m_status;
    MapPosition m_gridPos;  //現在の座標

    bool m_isAlive = true;  //生存しているか
    bool m_isDown = false;  //ダウン中か

    bool m_hasActed = false;   //行動済みか

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
    // 更新
    // ===================================================================
    void Update();

    // ===================================================================
    // 行動
    // ===================================================================
    void Move(const MapPosition& target);        //移動
    void ShadowMove(const MapPosition& target);  //影移動

    void Kill();                                 //死亡(影を踏まれる)

    void Attack(UnitComponent* target);          //攻撃
    void TakeDamage(int damage);                 //被ダメ
    void Down();                                 //ダウン状態

    void Place(const MapPosition& target);       //配置

    void BreakWall(const MapPosition& target);   //壁破壊   

    bool CanAct() const;    //行動可否
    bool IsAlive() const;   //生存判定
    bool IsDown() const;    //ダウン判定

    // ===================================================================
    // Getter / Setter
    // ===================================================================
    UnitCamp GetCamp() const { return m_status.camp; }
    //UnitState GetState() const { return m_status.state; }
    int GetHP() const { return m_status.hp; }
    void SetCamp(UnitCamp camp) { m_status.camp = camp; }

    const MapPosition& GetGridPos() const { return m_gridPos; }
    void SetGridPos(const MapPosition& pos) { m_gridPos = pos; }
};
