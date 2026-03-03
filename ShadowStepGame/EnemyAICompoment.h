#pragma once
#include "UnitComponent.h"
#include "UnitCommon.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include "ShadowSystemComponent.h"

// Edited by Yamanaka: 
///<summary>
///敵AIクラス。フローチャート通りに動く実装に変更している。
///</summary>

class EnemyAI
{
private:
	// ===================================================================
	// 定数
	// ===================================================================
	static constexpr int DETECTION_RANGE = 10;  // 索敵範囲（マンハッタン距離）
	static constexpr int CLOSE_RANGE = 4;       // 近距離判定の閾値

	// 歩行可能タイル判定
	bool IsWalkableTile(int tile) const
	{
		return tile == static_cast<int>(EMapTile::Empty)
			|| tile == static_cast<int>(EMapTile::Shadow)
			|| tile == static_cast<int>(EMapTile::Load);
	}

	// マップタイル取得（範囲外はNone）
	int GetTile(const int* const* mapData, int unitX, int unitZ, int width, int height) const
	{
		int mapX = unitX + width / 2;
		int mapZ = unitZ + height / 2;

		if (mapX < 0 || mapZ < 0 || mapX >= width || mapZ >= height)
			return static_cast<int>(EMapTile::None);
		return mapData[mapZ][mapX];
	}

public:
	// ===================================================================
	// メイン判断関数
	// ===================================================================
	UnitAction DecideAction(
		UnitComponent* enemy,
		const std::vector<UnitComponent*>& playerList,
		const std::vector<UnitComponent*>& enemyList,
		const std::vector<UnitComponent*>& allUnits,
		MapPosition sunDirection,
		const int* const* mapData,
		int mapWidth,
		int mapHeight
	);

private:
	// ===================================================================
	// ① 索敵：周囲10マス以内の最近プレイヤーを探す
	// ===================================================================
	UnitComponent* FindNearestPlayerInRange(
		UnitComponent* enemy,
		const std::vector<UnitComponent*>& playerList
	) const;

	// ===================================================================
	// ② 影判定：影が自分と同じ側にあるか
	// ===================================================================
	bool IsShadowOnSameSide(
		UnitComponent* enemy,
		UnitComponent* target,
		MapPosition sunDirection
	) const;

	// ===================================================================
	// ③ 攻撃判定：XまたはZが等しく隣接しているか
	// ===================================================================
	bool CanAttack(
		UnitComponent* enemy,
		UnitComponent* target
	) const;

	// ===================================================================
	// 移動先計算：差が大きい軸方向に1マス移動
	//   toward=true: 対象に近づく
	//   toward=false: 対象から遠ざかる
	// ===================================================================
	MapPosition CalcMoveOneStep(
		MapPosition from,
		MapPosition to,
		bool toward,
		const std::vector<UnitComponent*>& allUnits,
		UnitComponent* self,
		const int* const* mapData,
		int mapW, int mapH
	) const;

	// ===================================================================
	// 影の位置を計算
	// ===================================================================
	MapPosition CalcShadowPosition(
		UnitComponent* target,
		MapPosition sunDirection,
		int shadowLength
	) const;

	// ===================================================================
	// マンハッタン距離
	// ===================================================================
	int ManhattanDist(MapPosition a, MapPosition b) const
	{
		return abs(a.x - b.x) + abs(a.z - b.z);
	}
};