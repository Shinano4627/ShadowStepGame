#pragma once
#include "Component.h"
#include "UnitCommon.h"
#include "GameSystemComponent.h"
#include "UnitSystemComponent.h"
#include "input.h"
#include <vector>
// ===================================================================
// ユニットコンポネント
// ユニット一体分の状態・ターン管理
// ===================================================================

class UnitComponent :public Component
{
public:
    //--型定義--//
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
    Input m_input;


    UnitStatus m_status;
    MapPosition m_gridPos;  //現在の座標

    bool m_isAlive = true;  //生存しているか
    bool m_isDown = false;  //ダウン中か

    bool m_hasActed = false;   //行動済みか

    bool m_isMoveSelecting = false; //移動選択中か
    MapPosition m_moveTarget;       //移動モード用候補マス

    bool m_isPlacing = false;   //配置モード中か
    MapPosition m_placeTarget;      //配置候補マス

    bool m_isAttacking = false;
    UnitComponent* m_attackTarget = nullptr;

public:
 
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    UnitComponent();
    // ===================================================================
    // デストラクタ
    // ===================================================================
    ~UnitComponent();


    // ===================================================================
    // 更新
    // ===================================================================
    void Update();


    // ===================================================================
    // 行動
    // ===================================================================
    void BeginMove();                            //移動選択開始
    void BeginAttack();                          //攻撃開始
    void BeginPlace();                           //配置選択

    void Move(const MapPosition& target);        //移動
    void Attack(UnitComponent* target);          //攻撃

    void ShadowMove(const MapPosition& target);  //影移動
    void Kill();                                 //死亡(影を踏まれる)
    void TakeDamage(int damage);                 //被ダメ
    void Down();                                 //ダウン状態

    void TryPlaceObstacle();                     //配置確定用関数
    void PlaceObstacle();                        //オブジェクト生成

    void BreakWall(const MapPosition& target);   //壁破壊   

    // ===================================================================
    // 状態
    // ===================================================================
    bool CanAct() const;    //行動可否
    bool IsAlive() const;   //生存判定
    bool IsDown() const;    //ダウン判定
    void ResetTurn();       //ターンリセット(デバッグ用)

    // ===================================================================
    // 座標
    // ===================================================================
    Vector3 GridToWorld(const MapPosition& grid) const;   //座標返還

    
    

    

    

    // ===================================================================
    // Getter / Setter
    // ===================================================================
    UnitCamp GetCamp() const { return m_status.camp; }
    //UnitState GetState() const { return m_status.state; }
    int GetHP() const { return m_status.hp; }
    void SetCamp(UnitCamp camp) { m_status.camp = camp; }

    const MapPosition& GetGridPos() const { return m_gridPos; }
    void SetGridPos(const MapPosition& pos) { m_gridPos = pos; }

    void SetUnitId(int id) { m_status.id = id; }
    int GetUnitId()const { return m_status.id; }

 private:
    // ===================================================================
    // 内部更新
    // ===================================================================
    void UpdateMoveSelecting();                  //移動選択中
    void UpdateAttacking();                      //攻撃選択中
    void UpdatePlacing();                        //配置モード
};
