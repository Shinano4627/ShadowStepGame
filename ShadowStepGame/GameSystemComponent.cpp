//=======================================
// GameSystemComponent実装
//=======================================

#include "GameSystemComponent.h"
#include "GameObject.h"

// 他システム
#include "MapSystemComponent.h"
#include "UnitSystemComponent.h"
#include "SunManageComponent.h"
#include "ShadowSystemComponent.h"

void GameSystemComponent::Init()
{
    // 同じ GameObject にある他の SystemComponent を取得
    m_mapSystem = m_pOwner->GetComponent<MapSystemComponent>();
    m_unitSystem = m_pOwner->GetComponent<UnitSystemComponent>();
    m_sunSystem = m_pOwner->GetComponent<SunManageComponent>();
    int map_w = m_mapSystem->GetMapWidth();
    int map_h = m_mapSystem->GetMapHeight();
    m_shadowSystem = m_pOwner->GetComponent<ShadowSystemComponent>();
    m_shadowSystem->SetUp(map_w,map_h);

    // 安全チェック
    if (!m_mapSystem)  std::cout << "[GameSystem] MapSystemComponent が見つかりません！\n";
    if (!m_unitSystem) std::cout << "[GameSystem] UnitSystemComponent が見つかりません！\n";
    if (!m_sunSystem)  std::cout << "[GameSystem] SunManageComponent が見つかりません！\n";
    if (!m_shadowSystem) std::cout << "[GameSystem] ShadowSystemComponent が見つかりません！\n";

    // 状態データ初期化
    m_TurnCount = 0;
    m_TimelineIndex = 0;

    // 次のBattleStateへ
    ChangeState(BattleState::Init);
}

void GameSystemComponent::Update()
{
    // VK_E が押されたら状態更新（テスト用）
    if (IO_MANAGER.GetKeyDownKeyBord(VK_E))
    {
        // ターン状態に合わせた関数を呼び出し
        UpdateState();
    }
}

void GameSystemComponent::ChangeState(BattleState next)
{
    std::cout
        << "[GameSystem] Turn " << m_TurnCount
        << " : " << BattleStateToString(m_State)
        << " -> " << BattleStateToString(next)
        << std::endl;

    // 今のStateを保存
    m_beforeState = m_State;
    // 次のStateへ変更
    m_State = next;
}


void GameSystemComponent::UpdateState()
{
    // 現在のターン状態に合わせて関数を呼び出し
    switch (m_State)
    {
    case BattleState::Init:          UpdateInit(); break;
    case BattleState::TurnStart:     UpdateTurnStart(); break;
    case BattleState::UnitSelect:    UpdateUnitSelect(); break;
    case BattleState::UnitActionSelect: UpdateUnitActionSelect(); break;
    case BattleState::UnitActing:    UpdateUnitActing(); break;
    case BattleState::UnitEnd:       UpdateUnitEnd(); break;
    case BattleState::TurnEnd:       UpdateTurnEnd(); break;
    case BattleState::SunMove:       UpdateSunMove(); break;
    case BattleState::Judge:         UpdateJudge(); break;
    case BattleState::End:           UpdateEnd(); break;
    }
}

//=======================================
// BattleState:Init
// Scene開始、初期化直後にすべき処理
//=======================================
void GameSystemComponent::UpdateInit()
{
    // 戦闘ターンを始める
    ChangeState(BattleState::TurnStart);
}

//=======================================
// BattleState:TurnStart
// Turn開始。各ターンの最初にすべき処理
//=======================================
void GameSystemComponent::UpdateTurnStart()
{
    // 現在ターンの加算
    m_TurnCount++;

    // タイムライン作成
    BuildTimeline();
    m_TimelineIndex = 0;

    // 次のBattleStateへ
    ChangeState(BattleState::UnitSelect);
}

//=======================================
// BattleState:UnitSelect
// タイムラインから次に動くユニットを決定
//=======================================
void GameSystemComponent::UpdateUnitSelect()
{
    // 現在のタイムラインを取得
    Timeline* cur = GetCurrentTimeline();

    // タイムラインがなければターン終了
    if (!cur)
    {
        ChangeState(BattleState::TurnEnd);
        return;
    }

    // ============================
    // 太陽のターンかどうか
    // ============================
    if (cur->actorType == TimelineActorType::Sun)
    {
        ChangeState(BattleState::SunMove);
        return;
    }

    // ============================
    // ユニットのターン
    // ============================
    auto* unit = cur->unit;

    // ユニットがいるかのチェック
    if (!unit || unit->isDown == true)
    {
        NextTimeline();
        return;
    }

    m_CurrentUnit = unit;

    // ============================
    // プレイヤー or 敵で分岐
    // ============================
    if (unit->type == UnitType::Player)
    {
        ChangeState(BattleState::UnitActionSelect); // プレイヤー入力待ち
    }
    else
    {
        ChangeState(BattleState::UnitActing); // 敵AI行動
    }
}

//=======================================
// BattleState:UnitActionSelect
// プレイヤー入力待ち状態
//=======================================
void GameSystemComponent::UpdateUnitActionSelect()
{
    // TODO: プレイヤー入力待ち
    // m_CurrentUnit->StartActionInput(); など
}

//=======================================
// BattleState:UnitActing
// ユニットの行動処理中
//=======================================
void GameSystemComponent::UpdateUnitActing()
{
    // TODO: 敵AI行動開始
    // m_CurrentUnit->ExecuteAIAction(); など
}

//=======================================
// BattleState:UnitEnd
// 現在タイムラインのユニット行動終了
//=======================================
void GameSystemComponent::UpdateUnitEnd()
{
    m_CurrentUnit = nullptr;
    // タイムラインを確認し全てのユニット操作完了か調べる
    NextTimeline();
}

//=======================================
// BattleState:TurnEnd
// ターン終了処理
//=======================================
void GameSystemComponent::UpdateTurnEnd()
{
    ChangeState(BattleState::SunMove);
}

void GameSystemComponent::UpdateSunMove()
{
    // 太陽進行
    m_sunSystem->AdvanceTurn();

    // 影計算
    ShadowParam param =
        m_shadowSystem->CalcShadowParm(
            m_sunSystem->GetCurPosX(),
            m_sunSystem->GetCurPosZ(),
            m_mapSystem->GetMapWidth(),
            m_mapSystem->GetMapSizeHeight()
        );

    m_shadowSystem->UpdateShadowMap(
        m_mapSystem->GetRawMapData(),
        param
    );

    // マップ更新
    m_mapSystem->UpdateMap(
        m_unitSystem->GetAllUnits(),
        m_shadowSystem->GetShadowMap()
    );

    // 次は勝敗判定へ
    ChangeState(BattleState::Judge);
}

void GameSystemComponent::UpdateJudge()
{

    // 勝敗確定
    if (IsEnemyAllDead() || IsPlayerAllDead())
    {
        ChangeState(BattleState::End);
    }
    
    // 未決着
    if (m_beforeState == BattleState::TurnEnd)
    {
        ChangeState(BattleState::TurnStart);
    }
    else
    {
        // UnitEnd から来た
        NextTimeline();
        ChangeState(BattleState::UnitSelect);
    }
}

void GameSystemComponent::UpdateEnd()
{

}

void GameSystemComponent::BuildTimeline()
{
    m_Timeline.clear();

    // ユニット
    auto units = m_unitSystem->GetUnitsSortedBySpeed();
    for (auto* u : units)
    {
        Timeline t;
        t.unit = u;
        switch (u->type)
        {
        case UnitType::Enemy:
            t.actorType = TimelineActorType::Enemy;
            break;
        case UnitType::Player:
            t.actorType = TimelineActorType::Player;
            break;
        }
        t.speed = u->speed;
        m_Timeline.push_back(t);
    }

    // 太陽
    Timeline sun;
    sun.unit = nullptr;
    sun.actorType = TimelineActorType::Sun;
    sun.speed = -1;
    m_Timeline.push_back(sun);

}

// Indexが範囲内なら返す
GameSystemComponent::Timeline*
GameSystemComponent::GetCurrentTimeline()
{
    if (m_TimelineIndex < 0 || m_TimelineIndex >= static_cast<int>(m_Timeline.size()))
    {
        return nullptr;
    }

    return &m_Timeline[m_TimelineIndex];
}

// Indexを進める→TurnEnd
void GameSystemComponent::NextTimeline()
{
    //=======================================
    // Map/Shadow更新
    //=======================================
    // 影計算
    ShadowParam param =
        m_shadowSystem->CalcShadowParm(
            m_sunSystem->GetCurPosX(),
            m_sunSystem->GetCurPosZ(),
            m_mapSystem->GetMapWidth(),
            m_mapSystem->GetMapSizeHeight()
        );
    // 影マップ更新
    m_shadowSystem->UpdateShadowMap(
        m_mapSystem->GetRawMapData(),
        param
    );
    // マップ更新
    m_mapSystem->UpdateMap(
        m_unitSystem->GetAllUnits(),
        m_shadowSystem->GetShadowMap()
    );

    //=======================================
    // タイムラインを進める
    //=======================================
    m_TimelineIndex++;

    //=======================================
    // ターン終了 or 継続判定
    //=======================================
    if (m_TimelineIndex >= static_cast<int>(m_Timeline.size()))
    {
        // 全員行動終了
        ChangeState(BattleState::TurnEnd);
    }
    else
    {
        // まだ行動待ちユニットが残っている
        ChangeState(BattleState::Judge);
    }
}

// 勝敗判定
bool GameSystemComponent::IsPlayerAllDead() const
{
    return m_unitSystem->IsPlayerAllDead();
}

bool GameSystemComponent::IsEnemyAllDead() const
{
    return m_unitSystem->IsEnemyAllDead();
}

const char* GameSystemComponent::BattleStateToString(BattleState state)
{
    switch (state)
    {
    case BattleState::Init:              return "Init";
    case BattleState::TurnStart:         return "TurnStart";
    case BattleState::UnitSelect:        return "UnitSelect";
    case BattleState::UnitActionSelect:  return "UnitActionSelect";
    case BattleState::UnitActing:        return "UnitActing";
    case BattleState::UnitEnd:           return "UnitEnd";
    case BattleState::SunMove:           return "SunMove";
    case BattleState::TurnEnd:           return "TurnEnd";
    case BattleState::Judge:             return "Judge";
    case BattleState::End:               return "End";
    default:                             return "Unknown";
    }
}
