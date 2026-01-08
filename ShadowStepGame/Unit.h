#pragma once
#include <DirectXMath.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

// ユニットの所属
enum class UnitCamp
{
    Player, //味方
    Enemy   //敵

    //中立等の拡張要検討
};

// ユニットの種類
enum class UnitType
{
    Normal,     //通常ユニット
    Attack,     //攻撃ユニット
    Placement,  //配置ユニット
    Giant       //巨人
};

// ユニットの状態(仮)
enum class UnitState
{
    Idle,       //行動可能(ターンが来たら動ける)
    Acting,     //行動中
    Done,       //行動終了
    Disabled    //行動不能(攻撃を受けた)(休み)
};

class UnitModel;

class ShadowMap;    //前方宣言

class Unit
{
public:
    //--ライフサイクル--//
    Unit();             //インストラクタ
    virtual ~Unit();    //デストラクタ

    bool IsMyTurn() const;      //自ターンか判定
    virtual void StartTurn();   //ターン開始(行動リセット)
    virtual void Update();      //更新
    virtual void Draw() const;  //描画

    //--行動--//
    bool CanAct() const;    //行動可能判定

    //移動処理
    virtual void MoveTo(const XMINT2& gridPos); //移動
    virtual bool CanMove() const;               //移動可能判定
    virtual void EndTurn();                     //ターン終了
    bool HasFinishedTurn() const;               //終了判定

    //被ダメ
    virtual void TakeDamage(int damage);
    //行動不能にする(攻撃を受けた結果)
    virtual void Disable(); 

    //影移動
    virtual bool CanShadowMove(const XMINT2& targetGridPos, const ShadowMap& shadowMap) const;  //影移動が可能か
    virtual void ShadowMove(const XMINT2& targetGridPos, const ShadowMap& shadowMap);           //影移動実行

    //--情報取得--//
    UnitType GetType() const { return m_type; }
    UnitCamp GetCamp() const { return m_camp; }
    UnitState GetState() const { return m_state; }
    int GetSpeed() const { return m_speed; }
    int GetHP() const { return m_hp; }
    int GetMaxHP() const { return m_maxHp; }
    bool isHPZero() const { return m_hp <= 0; }
    const XMINT2& GetGridPos() const { return m_gridPos; }
    const Matrix& GetWorldMatrix() const { return m_world; }
   
    void SetModel(UnitModel* model) { m_model = model; }

protected:
    //--内部処理--//
    //ワールド行列更新
    void UpdateWorldMatrix();
    //マンハッタン距離
    //Unitを持っていない場所でも計算がしたい場合はこの関数をstaticに
    int CalcGridDistance(const XMINT2& a, const XMINT2& b) const;
    //自分から指定マスまでの距離
    int CalcDistanceFromSelf(const XMINT2& target) const;

    // グリッド座標
    XMINT2     m_gridPos;   //マス座標
    // ワールド行列（3D描画用）
    Matrix     m_world;     //ワールド行列
    //モデル描画用
    UnitModel* m_model;

    // パラメータ
    int        m_moveRange;     //移動可能距離
    int        m_speed;         //素早さ
    int        m_hp;            //現在HP
    int        m_maxHp;         //最大HP

    //行動不能管理
    bool m_disabledThisTurn = false;    //このターン行動不能か

    // ユニット識別
    UnitCamp   m_camp;  //所属
    UnitType   m_type;  //ユニット種類
    UnitState  m_state; //行動状態

    //プロトタイプ用ターン判定フラグ
    bool m_isMyTurn = false;
};
