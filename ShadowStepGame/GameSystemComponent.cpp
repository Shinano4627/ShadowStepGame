//=======================================
// GameSystemComponent実装
//=======================================

#include "GameSystemComponent.h"
#include "OrbitCameraComponent.h"
#include "GameObject.h"
#include "SoundManager.h"

// 他システム
#include "MapSystemComponent.h"
#include "UnitSystemComponent.h"
#include "SunManageComponent.h"
#include "ShadowSystemComponent.h"
#include "UISystemComponent.h"
#include "UnitComponent.h"
#include "EnemyAICompoment.h"
#include <unordered_set>
#include <SceneManager.h>

void GameSystemComponent::InitGame(std::unique_ptr<GameObjectList>& gameObjectList)
{
    GameEnd_flg = false;

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
        m_SelectPhase = SelectPhase::Init; // カメラ移動完了を待つ

        // 戦術視点カメラ指定位置を計算して移動開始
        if (m_orbitCamera)
        {
            Vector3 unitPos(
                m_Unitposition.x * 5.0f,
                0.0f,
                m_Unitposition.z * 5.0f
            );

            Vector3 tacticalDir(0.0f, 1.0f, -1.2f);
            tacticalDir.Normalize();
            float tacticalDistance = 50.0f;

            Vector3 endCamPos = unitPos + tacticalDir * tacticalDistance;
            m_orbitCamera->StartMoveTo(endCamPos, unitPos);
        }

        // SelectMap起動
        m_mapSystem->StartSelectMap(unit, m_SelectMapPosition);
        
        ChangeState(BattleState::UnitActionSelect); // プレイヤー入力待ち
    }

    else if(unit->GetType() == UnitType::Enemy)
    {
        // EnemyAI
        EnemyAI enemyAI;

        // Map 情報取得
        const int* const* mapData = m_mapSystem->GetRawMapData();
        int mapW = m_mapSystem->GetMapSizeHeight();
        int mapH = m_mapSystem->GetMapSizeWidth();

        // 太陽方向（SunManage などから）
        MapPosition sunDir = m_sunSystem->GetDirection();

        // Player 一覧
        const auto& players = m_unitSystem->GetPlayerUnits();
        const auto& enemys = m_unitSystem->GetEnemyUnits();
        const auto& units = m_unitSystem->GetAllUnits();

        UnitAction action = enemyAI.DecideAction(
            m_CurrentUnit,
            players,
            enemys,
            units,
            sunDir,
            mapData,
            mapW,
            mapH
        );

        // Unitに行動をセット
        m_CurrentUnit->SetAction(action);

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

    switch (m_SelectPhase)
    {
    case SelectPhase::Init:
    {
        // カメラ移動が完了しているか
        if (m_orbitCamera->IsMoveFinished())
        {
            m_SelectPhase = SelectPhase::Action;
        }
    }
    case SelectPhase::Action:
    {
        // UIシステムから取得
        if (IO_MANAGER.GetKeyDownKeyBord(VK_LBUTTON))   // 左クリックで更新
        {
            m_SelectType = m_uISystem->GetSelectedButton();
            m_SelectPhase = SelectPhase::Position;
            SOUND_MANAGER.PlaySE(SOUND_LABEL_SE_ACTION_SELECTED);
        }
    }
    break;
    case SelectPhase::Position:
    {
        // 入力管理
        // セレクト移動（仮実装：カメラがマップを右上を正、左下を負と見ていると仮定）
        Input_Select();

        // 選択確定(F)
        if (IO_MANAGER.GetKeyDownKeyBord(VK_E) || IO_MANAGER.GetKeyDown(TYPE_OK))
        {
            bool ok = false;
            // Actionに合わせてMapの位置をチェック(あとで関数化)
            switch (m_SelectType)
            {
            case UnitActionType::Move:
                ok = m_mapSystem->IsWalkableAtUnitPos(
                    m_SelectMapPosition.x,
                    m_SelectMapPosition.z);
                if (ok)
                {
                    // 行動
                    m_CurrentUnit->Move();
                }
                break;

            case UnitActionType::Attack:
                ok = m_mapSystem->IsAttackableAtUnitPos(
                    m_Unitposition.x, m_Unitposition.z,
                    m_SelectMapPosition.x, m_SelectMapPosition.z,
                    m_UnitType);

                if (ok)
                {
                    // 行動
                    m_CurrentUnit->Attack();
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

                if (ok)
                {
                    // 行動
                    m_CurrentUnit->Place();
                }
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

                ChangeState(BattleState::UnitActing);

                if (m_SelectType == UnitActionType::Move) {
                    m_Unitposition = m_SelectMapPosition;
                }

                SOUND_MANAGER.PlaySE(SOUND_LABEL_SE_POSITION_SELECTED);
            }
            //else {
            //    m_SelectPhase == SelectPhase::Action;
            //    ok = false;
            //}
        }
        break;

    default:
        break;
    }
    }
}

//=======================================
// BattleState:UnitActing
// ユニットの行動処理中
//=======================================
void GameSystemComponent::UpdateUnitActing()
{
    if (!m_CurrentUnit->IsTurnFinished())
    {
        // 行動を実行
        m_CurrentUnit->ExcuteAction();
    }
    else
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
    CheckShadowKill();
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
    ChangeState(BattleState::UnitEnd);
}

void GameSystemComponent::UpdateJudge(GameObjectList* gameObjectList)
{

    // 勝敗確定
    if (IsEnemyAllDead() || IsPlayerAllDead())
    {
        if (IsEnemyAllDead())
        {
            // PlayerWIN
            SCENE_MANAGER.SetPlayerResult(true);
        }
        else {
            // PlayerLOSS
            SCENE_MANAGER.SetPlayerResult(false);
        }
        ChangeState(BattleState::End);
    }
    else {
        // タイムラインを確認し全てのユニット操作完了か調べる
        NextTimeline(gameObjectList);
    }
}

void GameSystemComponent::UpdateEnd()
{
    GameEnd_flg = true;
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
        m_shadowSystem->GetShadowMap(),
        gameObjectList
    ); 
    
    // 影更新
    UpdateShadow(gameObjectList);

    // マップ更新
    m_mapSystem->UpdateMap(
        m_unitSystem->GetAllUnits(),
        m_shadowSystem->GetShadowMap(),
        gameObjectList
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
        SOUND_MANAGER.PlaySE(SOUND_LABEL_SE_CURSOL_MOVE);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_LEFT))
    {
        m_SelectMapPosition.x -= 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
        SOUND_MANAGER.PlaySE(SOUND_LABEL_SE_CURSOL_MOVE);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_UP))
    {
        m_SelectMapPosition.z += 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
        SOUND_MANAGER.PlaySE(SOUND_LABEL_SE_CURSOL_MOVE);
    }
    else if (IO_MANAGER.GetKeyDownKeyBord(VK_DOWN))
    {
        m_SelectMapPosition.z -= 1;
        m_mapSystem->UpdateSelectCursor(m_SelectMapPosition);
        SOUND_MANAGER.PlaySE(SOUND_LABEL_SE_CURSOL_MOVE);
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

void GameSystemComponent::CheckShadowKill()
{
    // 太陽方向から影ベクトル作成
    MapPosition lightDir = m_sunSystem->GetDirection();

    // 真上 → 影なし
    if (lightDir.x == 0 && lightDir.z == 0)
        return;

    int distance = abs(lightDir.x) + abs(lightDir.z);
    int shadowLength = std::clamp(distance / 2, 2, 8);

    MapPosition shadowDir{
        (lightDir.x != 0) ? (lightDir.x > 0 ? 1 : -1) : 0,
        (lightDir.z != 0) ? (lightDir.z > 0 ? 1 : -1) : 0
    };

    auto allPlayers = m_unitSystem->GetPlayerUnits();
    auto allEnemies = m_unitSystem->GetEnemyUnits();

    // 判定用ラムダ
    auto checkShadow = [&](const std::vector<UnitComponent*>& owners,
        const std::vector<UnitComponent*>& targets)
        {
            std::unordered_set<UnitComponent*> killSet;

            for (auto* owner : owners)
            {
                if (!owner) continue;

                MapPosition base = owner->GetPosition();

                // このユニットの影を計算
                std::vector<MapPosition> shadowPositions;
                for (int i = 1; i <= shadowLength; ++i)
                {
                    MapPosition shadowPos{
                        base.x + shadowDir.x * i,
                        base.z + shadowDir.z * i
                    };

                    int mx, mz;
                    if (!m_mapSystem->ConvertUnitPosToMapIndex(shadowPos.x, shadowPos.z, mx, mz))
                        break;

                    shadowPositions.push_back(shadowPos);
                }

                // 影の上に敵がいるか判定
                for (auto* target : targets)
                {
                    if (!target) continue;
                    MapPosition targetPos = target->GetPosition();

                    for (auto& sp : shadowPositions)
                    {
                        if (sp == targetPos)
                        {
                            // 敵が影の上にいたらこのユニットをKILL
                            killSet.insert(target);
                            break;
                        }
                    }
                }
            }

            // KILL処理
            for (auto* dead : killSet)
            {
                if (!dead) continue;

                std::cout << "[GameSystem] ShadowKill! UnitID:" << dead->GetId() << std::endl;

                // タイムラインから削除
                int idx = 0;
                while (idx < static_cast<int>(m_Timeline.size()))
                {
                    if (m_Timeline[idx].unit == dead)
                    {
                        m_Timeline.erase(m_Timeline.begin() + idx);
                        if (idx <= m_TimelineIndex && m_TimelineIndex > 0)
                            m_TimelineIndex--; // 現在インデックス補正
                    }
                    else
                    {
                        idx++;
                    }
                }

                // ユニットシステムから削除
                m_unitSystem->UnRegisterUnit(dead);
                dead->GetOwner()->SetActive(false);
            }

        };

    // Playerの影をEnemyが踏んでいるか → Playerが死ぬ
    checkShadow(allPlayers, allEnemies);

    // Enemyの影をPlayerが踏んでいるか → Enemyが死ぬ
    checkShadow(allEnemies, allPlayers);
}
