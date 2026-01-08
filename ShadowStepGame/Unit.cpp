#include "Unit.h"
#include "UnitModel.h"
//#include "ShadowMap.h"    //影情報管理クラスをインクルード
//#include "Renderer.h"

//---------------------
// コンストラクタ
//---------------------
Unit::Unit()
    : m_world(Matrix::Identity)
    , m_model(nullptr)
    , m_gridPos(0, 0)           //グリッド座標
    , m_moveRange(5)            //移動可能距離
    , m_speed(10)               //素早さ
    , m_camp(UnitCamp::Player)  //陣営(一旦デフォでプレイヤー)
    , m_type(UnitType::Normal)  //ユニットの種類
    , m_state(UnitState::Idle)  //状態
    , m_hp(5)                   //仮 ユニットごとにHP変更する場合は派生クラスのコンストラクタで上書き
    , m_maxHp(5)
{
    UpdateWorldMatrix();
}

//---------------------
// デストラクタ
//---------------------
Unit::~Unit()
{
}

//---------------------
// 自ターン判定
//---------------------
bool Unit::IsMyTurn() const
{
    if (m_state == UnitState::Disabled)
        return false;

    //TODO:
    //ターン管理(TurnManager等)で選ばれているかを返す

    //プロトタイプ用に簡易フラグ
    return m_state != UnitState::Disabled && m_isMyTurn;
}

//---------------------
// ターン開始処理
//---------------------
void Unit::StartTurn()
{
    //前のターンで攻撃されていた場合
    if (m_disabledThisTurn)
    {
        //このターンは行動不能のまま
        m_state = UnitState::Disabled;

        //HP回復
        m_hp = m_maxHp;

        //次のターンには解除されるようフラグをおろす
        m_disabledThisTurn = false;

        m_isMyTurn = false; //行動不可
        return;
    }

    m_state = UnitState::Idle;
    m_isMyTurn = true;  //行動可
}

//---------------------
// 更新処理
//---------------------
void Unit::Update()
{
    if (m_state == UnitState::Disabled)
    {
        return;
    }
    //（アニメーションや状態更新用）
}

//---------------------
// 描画
//---------------------
void Unit::Draw() const
{
    //描画不要な状態チェック
    if (!m_model) return;

    //ワールド行列を渡して描画
    m_model->Draw(m_world);
}

//---------------------
// 行動可能判定
//---------------------
bool Unit::CanAct() const
{
    //すべての行動の前にこの関数を通す
    
    return m_state == UnitState::Idle;
}

//---------------------
// 移動
//---------------------
void Unit::MoveTo(const XMINT2& gridPos)
{
    if (!CanAct())
        return;

    m_state = UnitState::Acting;

    m_gridPos = gridPos;
    UpdateWorldMatrix();

    //行動完了
    EndTurn();
}

bool Unit::CanMove() const
{
    if (!CanAct()) return false;

    //TODO:距離チェック
    return true;
}

void Unit::EndTurn()
{
    //すでに終了しているなら何もしない
    if (m_state == UnitState::Done)
        return;

    m_state = UnitState::Done;
    m_isMyTurn = false;
}

bool Unit::HasFinishedTurn() const
{
    //ターン管理ではif(unit->HasFinishedTurn())で判定すればok
    return m_state == UnitState::Done;
}

//---------------------
// 被ダメ関数
//---------------------
void Unit::TakeDamage(int damage)
{
    m_hp -= damage;

    if (m_hp <= 0)
    {
        m_hp = 0;

        //HP0になったら次のターン行動不能
        Disable();
    }
}

//---------------------
// 攻撃などで行動不能にする
//---------------------
void Unit::Disable()
{
    if (m_state == UnitState::Disabled)
        return;

    m_disabledThisTurn = true;
    m_state = UnitState::Disabled;
}

//---------------------
// ワールド行列更新
//---------------------
void Unit::UpdateWorldMatrix()
{
    // SRPG想定：1マス = 1.0f
    Vector3 worldPos(
        static_cast<float>(m_gridPos.x),
        0.0f,
        static_cast<float>(m_gridPos.y)
    );

    m_world = Matrix::CreateTranslation(worldPos);
}

//---------------------
// マンハッタン距離
//---------------------
int Unit::CalcGridDistance(const XMINT2& a, const XMINT2& b) const
{
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int Unit::CalcDistanceFromSelf(const XMINT2& target) const
{
    return CalcGridDistance(m_gridPos, target);
}

//---------------------
// 影移動判定
//---------------------
bool Unit::CanShadowMove(const XMINT2& targetGridPos, const ShadowMap& shadowMap) const
{
    if (!CanAct()) return false;
    //TODO:
    //1.現在位置の隣マスに影が存在するか
    //2.影がtargetGridPosまで連結しているか
    //3.targetGridPosが有効マスか(範囲外・障害物チェック)

    return true; //仮実装
}

void Unit::ShadowMove(const XMINT2& targetGridPos, const ShadowMap& shadowMap)
{
    if (!CanAct())
        return;

    if (!CanShadowMove(targetGridPos, shadowMap))
        return;

    m_state = UnitState::Acting;

    //TODO:
    //・影をたどる移動処理
    //・エフェクト・演出

    m_gridPos = targetGridPos;
    UpdateWorldMatrix();

    EndTurn();
}