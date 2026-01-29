#pragma once
#include "UnitComponent.h"
#include "UnitCommon.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"

class EnemyAI
{
private:
	// マップタイル定義（EnemyAI視点）
	static constexpr int TILE_EMPTY = static_cast<int>(EMapTile::Empty);
	static constexpr int TILE_SHADOW = static_cast<int>(EMapTile::Shadow);

	static constexpr int WalkableTiles[] =
	{
		TILE_EMPTY,
		TILE_SHADOW
	};

	bool IsWalkableTile(int tile) const
	{
		for (int t : WalkableTiles)
		{
			if (tile == t) return true;
		}
		return false;
	}

	int GetTile(
		const int* const* mapData,
		int x, int z,
		int width, int height
	) const
	{
		if (x < 0 || z < 0 || x >= width || z >= height)
			return static_cast<int>(EMapTile::None);

		return mapData[z][x];
	}

public:
	UnitAction DecideAction(
		UnitComponent* enemy,	// 操作ユニット
		const std::vector<UnitComponent*>& playerList,	// PlayerList
		MapPosition SunDirection,	// 太陽のMapPosition（ディレクションライト）
		const int* const* mapData,
		int mapWidth,
		int mapHeight
	);

private:
	// ターゲット選択
	UnitComponent* FindNearestPlayer(UnitComponent* enemy,
		const std::vector<UnitComponent*>& playerList) const;

	//=======================================
	// 判定関数
	//=======================================
	// ２：影踏み
	bool CanStepOnShadow(
		UnitComponent* enemy,
		UnitComponent* target,
		MapPosition sunDir,
		const int* const* mapData,
		int mapW,
		int mapH
	) const;

	// ３：攻撃
	bool CanAttack(
		UnitComponent* enemy,
		UnitComponent* target,
		const int* const* mapData,
		int mapW,
		int mapH
	) const;

	// ４：接近
	bool CanMove(
		UnitComponent* enemy
	) const;

	//=======================================
	// 処理関数
	//=======================================
	// ２：影踏み
	UnitAction MakeShadowKillAction(
		UnitComponent* enemy,
		UnitComponent* target,
		MapPosition sunDir
	) const;

	MapPosition CalcShadowPosition(
		UnitComponent* target,
		MapPosition sunDir
	) const;

	// ３：攻撃
	UnitAction MakeAttackAction(
		UnitComponent* enemy,
		UnitComponent* target
	) const;

	// ４：接近
	UnitAction MakeMoveCloserAction(
		UnitComponent* enemy,
		UnitComponent* target,
		const int* const* mapData,
		int mapW,
		int mapH
	) const;

	MapPosition DecideMoveCloser(
		UnitComponent* enemy,
		UnitComponent* target,
		const int* const* mapData,
		int mapW,
		int mapH
	) const;

};