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
#include "UISystemComponent.h"
#include "UnitComponent.h"

void GameSystemComponent::Init()
{
    // 同じ GameObject にある他の SystemComponent を取得
    m_mapSystem = m_pOwner->GetComponent<MapSystemComponent>();
    m_unitSystem = m_pOwner->GetComponent<UnitSystemComponent>();
    m_sunSystem = m_pOwner->GetComponent<SunManageComponent>();
    m_shadowSystem = m_pOwner->GetComponent<ShadowSystemComponent>();
    m_uISystem = m_pOwner->GetComponent<UISystemComponent>();

    // 安全チェック
    if (!m_mapSystem)
    {
        std::cout << "[GameSystem] MapSystemComponent が見つかりません！\n"; assert(false);
    }
    if (!m_unitSystem)
    {
        std::cout << "[GameSystem] UnitSystemComponent が見つかりません！\n"; assert(false);
    }
    if (!m_sunSystem)
    {
        std::cout << "[GameSystem] SunManageComponent が見つかりません！\n"; assert(false);
    }
    if (!m_shadowSystem)
    {
        std::cout << "[GameSystem] ShadowSystemComponent が見つかりません！\n"; assert(false);
    }
    if (!m_uISystem)
    {
        std::cout << "[GameSystem] UISystemComponent が見つかりません！\n"; assert(false);
    }

    // 初期処理実行
    int map_w = m_mapSystem->GetMapWidth();
    int map_h = m_mapSystem->GetMapHeight();
    m_shadowSystem->SetUp(map_w, map_h);

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
    if (!unit || unit->IsDown() == true)
    {
        NextTimeline();
        return;
    }

    m_CurrentUnit = unit;

    m_CurrentUnit->StartTurn();

    // ============================
    // プレイヤー or 敵で分岐
    // ============================
    if (unit->GetType() == UnitType::Player)
    {
        // Select関係変数
        m_SelectAction = false;
        m_SelectPosition = false;
        m_SelectMapPosition = unit->GetPosition();
        m_Unitposition = unit->GetPosition();
        m_UnitType = unit->GetType();
        m_SelectType = UnitActionType::None;
        // SelectMap起動
        m_mapSystem->StartSelectMap(unit, m_SelectMapPosition);
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
    // 全体の流れ（仮決定）
    /*
        1:ActionSelect。
            GameSystemで入力受付→UISystemに反映させつつ、チェックと決定。
        2:Position選択 ←完了
            MapSystemのSelectMap描画起動
            →GameSystemで入力受付（十字キー）
            →MapSystemのカーソルを反映させつつ、
            選択されたら問題ないかチェックする
            →問題なければUnitに指示だし
        3:UnitActへ
    */

    // Action未選択
    if (!m_SelectAction) {
        // 入力確認
        //UISystemからenumで選択を貰う

    }

    // まだポジション選択していない
    if (!m_SelectPosition)
    {
        // 入力管理
        // セレクト移動（仮実装：カメラがマップを右上を正、左下を負と見ていると仮定）
        Input_Select();
        // Mapに反映
        // m_mapSystem->SetSelectPosition(m_SelectMapPosition);

        // 選択確定(Enter)
        if (IO_MANAGER.GetKeyDown(TYPE_OK))
        {
            // Actionに合わせてMapの位置をチェック(あとで関数化)
            switch (m_SelectType)
            {
            case UnitActionType::Move:
                // OKか？
                if (m_mapSystem->IsWalkableAtUnitPos(
                    m_SelectMapPosition.x, m_SelectMapPosition.z
                )){

                    m_SelectPosition = true;
                }
                else {
                    // 実行できないので他を選択してもらう
                }

                break;
            case UnitActionType::Attack:
                // OKか？
                if (m_mapSystem->IsAttackableAtUnitPos(
                    m_Unitposition.x, m_Unitposition.z,  // Unit位置
                    m_SelectMapPosition.x, m_SelectMapPosition.z,    // 攻撃したい位置
                    m_UnitType  // Player or Enemy のどちらの処理か
                )) {
                    m_SelectPosition = true;
                }
                else {
                    // 実行できないので他を選択してもらう
                }

                break;
            case UnitActionType::Place:
                // OKか？
                if (m_mapSystem->IsPlacebleAtUnitPos(
                    m_SelectMapPosition.x, m_SelectMapPosition.z
                )) {

                    m_SelectPosition = true;
                }
                else {
                    // 実行できないので他を選択してもらう
                }
                break;
            }


        }
    }

    // 全て選択完了した
    if (m_SelectAction && m_SelectPosition)
    {
        // UnitActionを組み立て
        UnitAction action;
        action.type = m_SelectType;
        action.targetGrid = m_SelectMapPosition;

        // Unitに行動をセット
        m_CurrentUnit->SetAction(action);

        // SelectMap終了
        m_mapSystem->EndSelectMap();

        // 行動を実行
        m_CurrentUnit->ExcuteAction();


        // 次の状態へ
        ChangeState(BattleState::UnitActing);
        
    }
}

//=======================================
// BattleState:UnitActing
// ユニットの行動処理中
//=======================================
void GameSystemComponent::UpdateUnitActing()
{
    // Unit行動終了チェック
    if (m_CurrentUnit->IsTurnDinished())
    {
        m_CurrentUnit->EndTurn();
        ChangeState(BattleState::UnitEnd);
    }
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
        switch (u->GetType())
        {
        case UnitType::Enemy:
            t.actorType = TimelineActorType::Enemy;
            break;
        case UnitType::Player:
            t.actorType = TimelineActorType::Player;
            break;
        }
        t.speed = u->GetSpeed();
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

void GameSystemComponent::Input_Select()
{
    
    if (IO_MANAGER.GetKeyDownKeyBord(VK_UP))
    {
        m_SelectMapPosition.x += 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_DOWN))
    {
        m_SelectMapPosition.x -= 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_RIGHT))
    {
        m_SelectMapPosition.z += 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_LEFT))
    {
        m_SelectMapPosition.z -= 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
    }
    
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
