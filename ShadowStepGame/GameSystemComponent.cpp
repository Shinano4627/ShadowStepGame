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

void GameSystemComponent::InitGame(std::unique_ptr<GameObjectList>& gameObjectList)
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
    // 影の設定
    // GameObjectList(ShadowObject用)
    int map_w = m_mapSystem->GetMapWidth();
    int map_h = m_mapSystem->GetMapHeight();
    m_shadowSystem->SetUp(
        map_w,
        map_h,
        m_mapSystem->GetSizePiece(),
        m_mapSystem->GetDrawStartPosX(),
        m_mapSystem->GetDrawStartPosZ(),
        m_mapSystem->GetRawMapData(),
        gameObjectList.get()
    );
    UpdateShadow(gameObjectList.get());

    // 状態データ初期化
    m_TurnCount = 0;
    m_TimelineIndex = 0;

    // 次のBattleStateへ
    ChangeState(BattleState::Init);
}

void GameSystemComponent::UpdateGame(std::unique_ptr<GameObjectList>& gameObjectList)
{
    // VK_E が押されたら状態更新（テスト用）
    if (IO_MANAGER.GetKeyDownKeyBord(VK_E))
    {
        // ターン状態に合わせた関数を呼び出し
        // UpdateState();
    }

    // ターン状態に合わせた関数を呼び出し
    UpdateState(gameObjectList.get());
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


void GameSystemComponent::UpdateState(GameObjectList* gameObjectList)
{
    // 現在のターン状態に合わせて関数を呼び出し
    switch (m_State)
    {
    case BattleState::Init:          UpdateInit(); break;
    case BattleState::TurnStart:     UpdateTurnStart(); break;
    case BattleState::UnitSelect:    UpdateUnitSelect(gameObjectList); break;
    case BattleState::UnitActionSelect: UpdateUnitActionSelect(); break;
    case BattleState::UnitActing:    UpdateUnitActing(); break;
    case BattleState::UnitEnd:       UpdateUnitEnd(); break;
    case BattleState::TurnEnd:       UpdateTurnEnd(); break;
    case BattleState::SunMove:       UpdateSunMove(); break;
    case BattleState::Judge:         UpdateJudge(gameObjectList); break;
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
void GameSystemComponent::UpdateUnitSelect(GameObjectList* gameObjectList)
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
    if (!unit)
    {
        NextTimeline(gameObjectList);
        return;
    }
    if (unit->IsDown() == true) {
        unit->RecoverDown();
        NextTimeline(gameObjectList);
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
        m_UnitModel = unit->GetModel();
        m_SelectType = UnitActionType::None;
        m_SelectPhase = SelectPhase::Action;
        // SelectMap起動
        m_mapSystem->StartSelectMap(unit, m_SelectMapPosition);
        
        ChangeState(BattleState::UnitActionSelect); // プレイヤー入力待ち
    }
    else if(unit->GetType() == UnitType::Enemy)
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

    // ============================
    // Action入力（常に受け付ける）
    // ============================
    // UIシステムから取得
    if (IO_MANAGER.GetKeyDownKeyBord(VK_LBUTTON))   // 左クリックで更新
    {
        m_SelectType = m_uISystem->GetSelectedButton();
    }    

    // Actionが初めて選ばれた瞬間
    if (m_SelectType != UnitActionType::None &&
        m_SelectPhase == SelectPhase::Action)
    {
        m_SelectPhase = SelectPhase::Position;
    }

    // まだポジション選択していない
    if (m_SelectPhase == SelectPhase::Position)
    {
        // 入力管理
        // セレクト移動（仮実装：カメラがマップを右上を正、左下を負と見ていると仮定）
        Input_Select();
        
        // 選択確定(F)
        if (IO_MANAGER.GetKeyDownKeyBord(VK_F))
        {
            bool ok = false;
            // Actionに合わせてMapの位置をチェック(あとで関数化)
            switch (m_SelectType)
            {
            case UnitActionType::Move:
                ok = m_mapSystem->IsWalkableAtUnitPos(
                    m_SelectMapPosition.x,
                    m_SelectMapPosition.z);
                break;

            case UnitActionType::Attack:
                ok = m_mapSystem->IsAttackableAtUnitPos(
                    m_Unitposition.x, m_Unitposition.z,
                    m_SelectMapPosition.x, m_SelectMapPosition.z,
                    m_UnitType);

                if (ok)
                {
                    UnitComponent* target =
                        m_unitSystem->FindUnitAtPosition(m_SelectMapPosition);

                    if (target)
                    {
                        // 仮：2ターン行動不能
                        target->SetDown(2);
                    }
                }
                break;

            case UnitActionType::Place:
                ok = m_mapSystem->IsPlacebleAtUnitPos(
                    m_SelectMapPosition.x,
                    m_SelectMapPosition.z);
                break;
            }

            if (ok)
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

                ChangeState(BattleState::UnitActing);

                if (m_SelectType == UnitActionType::Move) {
                    m_Unitposition = m_SelectMapPosition;
                }
            }
            else {
                m_SelectPhase == SelectPhase::Action;
                ok = false;
            }

        }
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
    // 影チェック
    CheckShadowKill(m_CurrentUnit);

    m_CurrentUnit = nullptr;
    ChangeState(BattleState::Judge);
}

//=======================================
// BattleState:TurnEnd
// ターン終了処理
//=======================================
void GameSystemComponent::UpdateTurnEnd()
{
    ChangeState(BattleState::TurnStart);
}

void GameSystemComponent::UpdateSunMove()
{
    // 太陽進行
    m_sunSystem->AdvanceTurn();

    // 次は勝敗判定へ
    ChangeState(BattleState::Judge);
}

void GameSystemComponent::UpdateJudge(GameObjectList* gameObjectList)
{

    // 勝敗確定
    if (IsEnemyAllDead() || IsPlayerAllDead())
    {
        ChangeState(BattleState::End);
    }
    else {
        // タイムラインを確認し全てのユニット操作完了か調べる
        NextTimeline(gameObjectList);
    }
}

void GameSystemComponent::UpdateEnd()
{

}

void GameSystemComponent::UpdateShadow(GameObjectList* gameObjectList)
{
    ShadowParam param =
        m_shadowSystem->CalcShadowParm(
            m_sunSystem->GetDirection(),
            m_mapSystem->GetMapWidth(),
            m_mapSystem->GetMapSizeHeight()
        );

    m_shadowSystem->UpdateShadowMap(
        m_mapSystem->GetRawMapData(),
        param,
        gameObjectList
    );
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
void GameSystemComponent::NextTimeline(GameObjectList* gameObjectList)
{
    //=======================================
    // Map/Shadow更新
    //=======================================
    // マップ更新
    m_mapSystem->UpdateMap(
        m_unitSystem->GetAllUnits(),
        m_shadowSystem->GetShadowMap()
    ); 
    
    // 影更新
    UpdateShadow(gameObjectList);

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
        ChangeState(BattleState::UnitSelect);
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
    
    if (IO_MANAGER.GetKeyDownKeyBord(VK_RIGHT))
    {
        m_SelectMapPosition.x += 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_LEFT))
    {
        m_SelectMapPosition.x -= 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_UP))
    {
        m_SelectMapPosition.z += 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_DOWN))
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

// 現在のユニット位置が影に被っているかチェックしてKill
void GameSystemComponent::CheckShadowKill(UnitComponent* unit)
{
    // m_Unitpositionをm_mapSystemのConvertUnitPosToMapIndexでMap配列に変換。
    // ShadowMapSystemを使って今のPositionが影にかぶっているかチェック
    // 被っていたらその影の発生ObjectのEMapTileを調べる。
    // （ShadowSystemにて影Mapの更新の際、その影の元のObjectのMap配列とEMapTileの情報を保存しておく）
    // m_UnitTypeがEnemyで影のObjctがPlayerの時、
    // m_UnitTypeがPlayerで影のオブジェクトがEnemyの時のみ次のKill処理をする
    // その影の元のObjectのMap配列をm_mapSystemのConvertMapIndexToUnitPosを使ってUnit座標に変換
    // UnitSystemのFindUnitAtPositionを使ってKillしたいユニットのComponent*を取得。
    // それを使ってUnitSystemのUnRegisterUnitでKill（削除）
    
    if (!unit) return;
    int mapX, mapZ;
    if (!m_mapSystem->ConvertUnitPosToMapIndex(
        m_Unitposition.x, m_Unitposition.z,
        mapX, mapZ
    ))
    {
        return;
    }

    // 影の有無確認
    int shadowSource = m_shadowSystem->GetShadowSourceAt(mapX, mapZ);

    // 影の元タイプ判定
    UnitType shadowType = UnitType::Player;
    switch (shadowSource) {
    case 2:// Player
        shadowType = UnitType::Enemy;
        break;
    case 3:// Enemy
        shadowType = UnitType::Player;
        break;
    default:// それ以外
        return;
        break;
    }


    // 自分と異なるタイプの影のみKill対象
    if (unit->GetType() == shadowType) return;

    // マップ座標 → Unit座標に変換
    int targetUnitX, targetUnitZ;
    m_mapSystem->ConvertMapIndexToUnitPos(mapX, mapZ, targetUnitX, targetUnitZ);
    MapPosition targetPos{ targetUnitX, targetUnitZ };

    UnitComponent* targetUnit = m_unitSystem->FindUnitAtPosition(targetPos);
    if (!targetUnit) return;

    std::cout << "[GameSystem] ShadowKill! UnitID:" << targetUnit->GetId() << std::endl;

    m_unitSystem->UnRegisterUnit(targetUnit);

}
