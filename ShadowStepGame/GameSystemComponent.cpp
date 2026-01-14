//=======================================
// GameSystemComponent実装
//=======================================

#include "GameSystemComponent.h"
#include "GameObject.h"

// 他システム
#include "MapSystemComponent.h"
// #include "UnitSystemComponent.h"
#include "SunManageComponent.h"

void GameSystemComponent::Init()
{
    // 同じ GameObject にある他の SystemComponent を取得
    m_mapSystem = m_pOwner->GetComponent<MapSystemComponent>();
    // m_unitSystem = m_pOwner->GetComponent<UnitSystemComponent>();
    m_sunSystem = m_pOwner->GetComponent<SunManageComponent>();

    // 安全チェック
    if (!m_mapSystem)  std::cout << "[GameSystem] MapSystemComponent が見つかりません！\n";
    // if (!m_unitSystem) std::cout << "[GameSystem] UnitSystemComponent が見つかりません！\n";
    if (!m_sunSystem)  std::cout << "[GameSystem] SunManageComponent が見つかりません！\n";

    m_State = BattleState::Init;
    m_TurnCount = 0;
    m_TimelineIndex = 0;


    ChangeState(BattleState::Init);
}

void GameSystemComponent::Update()
{
    // VK_E が押されたら状態更新（テスト用）
    if (IO_MANAGER.GetKeyDownKeyBord(VK_E))
    {
        UpdateState();
    }
}

void GameSystemComponent::ChangeState(BattleState next)
{
    std::cout << "[GameSystem] State: "
        << static_cast<int>(m_State)
        << " -> "
        << static_cast<int>(next)
        << std::endl;

    m_State = next;
}


void GameSystemComponent::UpdateState()
{
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
    case BattleState::End:           break;
    }
}


void GameSystemComponent::UpdateInit()
{
    ChangeState(BattleState::TurnStart);
}

void GameSystemComponent::UpdateTurnStart()
{
    m_TurnCount++;
    BuildTimeline();
    m_TimelineIndex = 0;
    ChangeState(BattleState::UnitSelect);
}


void GameSystemComponent::UpdateUnitSelect()
{
    // 現在のタイムラインを取得
    Timeline* current = GetCurrentTimeline();

    // 安全チェック：範囲外ならターン終了
    if (!current)
    {
        ChangeState(BattleState::TurnEnd);
        return;
    }

    // ============================
    // 太陽のターンかどうか
    // ============================
    if (current->actorType == TimelineActorType::Sun)
    {
        // TODO: SunMove 処理は SunManageComponent が担当予定
        ChangeState(BattleState::SunMove);
        return;
    }

    // ============================
    // ユニットのターン
    // ============================
    Unit* unit = current->unit;

    // TODO: Unit クラスの IsAlive() メソッドで死亡チェックを行う予定
    if (!unit /* || !unit->IsAlive() */)
    {
        // 現状は死亡判定未実装のため、とりあえず次のタイムラインへ
        NextTimeline();
        return;
    }

    // TODO: m_CurrentUnit にセットして UnitActionSelect や UnitActing で使用予定
    // m_CurrentUnit = unit;

    // ============================
    // プレイヤー or 敵で分岐
    // ============================
    // TODO: Unit クラスに UnitType を持たせる予定（Player / Enemy）
    // TODO: 敵AIの行動判定は今後 Enemy AI システムで行う予定
    /*if (unit->GetUnitType() == UnitType::Player)
    {
        ChangeState(BattleState::UnitActionSelect); // プレイヤー入力待ち
    }
    else
    {
        ChangeState(BattleState::UnitActing); // 敵AI行動
    }*/

    // 現状未実装のため、とりあえずプレイヤー入力待ちにしておく
    ChangeState(BattleState::UnitActionSelect);
}



void GameSystemComponent::UpdateUnitActionSelect()
{
    // TODO: プレイヤー入力待ち
    // m_CurrentUnit->StartActionInput(); など
}

void GameSystemComponent::UpdateUnitActing()
{
    // TODO: 敵AI行動開始
    // m_CurrentUnit->ExecuteAIAction(); など
}

void GameSystemComponent::UpdateUnitEnd()
{

    NextTimeline();
}

void GameSystemComponent::UpdateTurnEnd()
{
    ChangeState(BattleState::SunMove);
}

void GameSystemComponent::UpdateSunMove()
{
    if (m_sunSystem)
    {
        // 太陽を1ターン進める
        m_sunSystem->AdvanceTurn(); // 自動でターン進行に応じて太陽を動かす
        // UpdateMap()
        // UpdateShadow()
    }

    // 次は勝敗判定へ
    ChangeState(BattleState::Judge);
}

void GameSystemComponent::UpdateJudge()
{
    if (IsEnemyAllDead() || IsPlayerAllDead())
    {
        ChangeState(BattleState::End);
    }
    else
    {
        ChangeState(BattleState::TurnStart);
    }
}

void GameSystemComponent::BuildTimeline()
{
    m_Timeline.clear();

    // =======================================
    // TODO:
    // UnitSystemComponent から以下を取得する
    // ・生存している全ユニット
    // ・各ユニットの speed 値
    //
    // 例:
    // auto units = m_unitSystem->GetAliveUnits();
    // for (auto* unit : units)
    // {
    //     Timeline entry;
    //     entry.unit = unit;
    //     entry.actorType = TimelineActorType::Unit;
    //     entry.speed = unit->GetSpeed();
    //     m_Timeline.push_back(entry);
    // }
    // speed 降順にソート
    // std::sort(m_Timeline.begin(), m_Timeline.end(),
    //     [](const Timeline& a, const Timeline& b) { return a.speed > b.speed; });
    // =======================================

    // =======================================
    // 仮実装:
    // 太陽だけを Timeline に追加
    // nullptr = 太陽
    // =======================================
    Timeline sunEntry;
    // sunEntry.unit = nullptr;
    sunEntry.actorType = TimelineActorType::Sun;
    sunEntry.speed = 0;

    m_Timeline.push_back(sunEntry);
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
    m_TimelineIndex++;

    // UpdateShadow()←ShadowSystem
    // 光源情報を元に全てのオブジェクトの影を更新する。
    // ↑を元に↓をやる
    // UpdateMap()←MapSystem
    // ユニットの位置、オブジェクトの位置、影の位置を取ってきて表示する
    // UnitSystemの情報とShadowSystemの情報を元にMapを更新する。

    // Map情報の更新はGameSystemで行う。

    if (m_TimelineIndex >= static_cast<int>(m_Timeline.size()))
    {
        // 全員行動終了
        ChangeState(BattleState::TurnEnd);
    }
    else
    {
        // まだ行動待ちユニットが残っている
        ChangeState(BattleState::UnitSelect);
    }
}

// 勝敗判定
bool GameSystemComponent::IsPlayerAllDead() const
{
    // =======================================
    // TODO:
    // UnitSystemComponent に問い合わせて
    // ・プレイヤーユニットが全滅しているか判定
    //
    // 例:
    // return m_unitSystem->IsPlayerAllDead();
    // =======================================

    return false;
}

bool GameSystemComponent::IsEnemyAllDead() const
{
    // =======================================
    // TODO:
    // UnitSystemComponent に問い合わせて
    // ・敵ユニットが全滅しているか判定
    //
    // 例:
    // return m_unitSystem->IsEnemyAllDead();
    // =======================================

    return false;
}
