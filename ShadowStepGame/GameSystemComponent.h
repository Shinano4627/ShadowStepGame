//=======================================
// GameSystemComponent.h
// ・ターン進行
// ・タイムライン管理
// ・現在操作可能なユニットの決定
// ・勝敗判定
// などを管理するComponentクラス
// 処理は各Systemに移譲する予定
//=======================================
#pragma once
#include "Component.h"

// 各システムの前方宣言
class MapSystemComponent;
class UnitSystemComponent;
class SunManageComponent;
class Unit;

class GameSystemComponent : public Component
{
public:
	// ゲーム進行管理State
	enum class BattleState
	{
		Init,	// 初期化直後
		TurnStart,	// ターン開始（タイムライン作成）
		UnitSelect,	// 次に動くユニットを決定
		UnitActionSelect,	// ユニットの操作入力
		UnitActing,	// ユニット行動実行中
		UnitEnd,	// ユニット行動終了処理
		TurnEnd,	// 全ユニット行動終了
		SunMove,	// 太陽移動＆影更新
		Judge,	// 勝敗判定
		End,	// 終了
	};

	// タイムライン
	enum class TimelineActorType
	{
		Unit,
		Sun
	};

	struct Timeline
	{
		Unit* unit;
		TimelineActorType actorType;
		int speed;	// 行動順決定用
	};

public:
	//=======================================
	// コンストラクタ
	//=======================================
	GameSystemComponent() = default;

	//=======================================
	// デストラクタ
	//=======================================
	~GameSystemComponent() = default;

	//=======================================
	// 初期化処理
	//=======================================
	void Init() override;

	//=======================================
	// 更新処理
	//=======================================
	void Update() override;


	BattleState GetBattleState() { return m_State; }
private:
	//=======================================
	// 状態管理関数
	//=======================================
	void ChangeState(BattleState next);	// 引数に進行状態変更
	void UpdateState();

	void UpdateInit();
	void UpdateTurnStart();
	void UpdateUnitSelect();
	void UpdateUnitActionSelect();
	void UpdateUnitActing();
	void UpdateUnitEnd();
	void UpdateTurnEnd();
	void UpdateSunMove();
	void UpdateJudge();

	//=======================================
	// タイムライン管理関数
	//=======================================
	
	// ユニットシステムから生存ユニットを取得
	// 速さ順に並べ、最後に太陽を追加
	void BuildTimeline();
	
	// 現在のタイムライン取得
	Timeline* GetCurrentTimeline();
	
	// 次のタイムラインへ
	void NextTimeline();

	//=======================================
	// 勝敗ジャッジ（UnitSystemに問い合わせる形の予定）
	//=======================================
	bool IsPlayerAllDead() const;
	bool IsEnemyAllDead() const;

private:
	//=======================================
	// システムコンポーネント参照
	//=======================================
	// Tag "MapSystem" から取得
	MapSystemComponent* m_mapSystem = nullptr;

	// Tag "UnitSystem" から取得
	// UnitSystemComponent* m_unitSystem = nullptr;

	// Tag "SunSystem" から取得
	SunManageComponent* m_sunSystem = nullptr;


	//=======================================
	// 状態データ
	//=======================================
	BattleState m_State;
	int m_TurnCount;

	//=======================================
	// タイムラインデータ
	//=======================================
	std::vector<Timeline> m_Timeline;
	int m_TimelineIndex;

	// Unit* m_CurrentUnit;


};