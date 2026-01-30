#include "EnemyAICompoment.h"
#include <cmath>

UnitAction EnemyAI::DecideAction(
	UnitComponent* enemy,	// 操作ユニット
	const std::vector<UnitComponent*>& playerList,	// PlayerList
	const std::vector<UnitComponent*>& enemyList,
	const std::vector<UnitComponent*>& allUnits,
	MapPosition SunDirection,	// 太陽のMapPosition（ディレクションライト）
	const int* const* mapData,
	int mapWidth,
	int mapHeight
)
{
	// １：Playerから最も近いPlayerを検索
	UnitComponent* target = FindNearestPlayer(enemy, playerList, enemyList);

	UnitAction none;
	none.targetGrid = MapPosition(0, 0);
	none.type = UnitActionType::None;
	if (!target) {
		return none;
	}

	// ２：影を踏めるか？
	if (CanStepOnShadow(enemy, target, SunDirection, mapData, mapWidth, mapHeight))
		return MakeShadowKillAction(enemy, target, SunDirection);

	// ３：攻撃できるか？
	if (CanAttack(enemy, target,mapData,mapWidth,mapHeight))
		return MakeAttackAction(enemy, target);

	// ４：近づけるか？
	if (CanMove(enemy))
		return MakeMoveCloserAction(enemy, target, allUnits,mapData, mapWidth, mapHeight);

	// ５：何もできない
	return none;
}

UnitComponent* EnemyAI::FindNearestPlayer(
	UnitComponent* enemy,
	const std::vector<UnitComponent*>& playerList,
	const std::vector<UnitComponent*>& enemyList
) const
{
	if (!enemy) return nullptr;

	// 操作ユニット情報取得
	MapPosition epos = enemy->GetPosition();

	// 変数宣言
	UnitComponent* best = nullptr;
	int bestDist = INT_MAX;

	for (auto* p : playerList)
	{
		if (!p) continue;

		MapPosition ppos = p->GetPosition();

		// 他の敵ユニットと衝突しないかも確認（任意）
		bool blocked = false;
		for (auto* otherEnemy : enemyList)
		{
			if (!otherEnemy || otherEnemy == enemy) continue;
			if (ppos == otherEnemy->GetPosition())
			{
				blocked = true;
				break;
			}
		}

		if (blocked) continue;

		int dist =
			abs(epos.x - ppos.x) +
			abs(epos.z - ppos.z);

		if (dist < bestDist)
		{
			bestDist = dist;
			best = p;
		}
	}
	return best;
}

bool EnemyAI::CanStepOnShadow(
	UnitComponent* enemy,
	UnitComponent* target,
	MapPosition sunDir,
	const int* const* mapData,
	int mapW,
	int mapH
) const
{
	if (!enemy || !target) return false;

	MapPosition shadow = CalcShadowPosition(target, sunDir);

	int tile = GetTile(mapData, shadow.x, shadow.z, mapW, mapH);

	// 影マスに到達できるか？
	if (!IsWalkableTile(tile))
		return false;

	MapPosition epos = enemy->GetPosition();
	return abs(epos.x - shadow.x) <= 1 &&
		abs(epos.z - shadow.z) <= 1;
}

bool EnemyAI::CanAttack(
	UnitComponent* enemy,
	UnitComponent* target,
	const int* const* mapData,
	int mapW,
	int mapH
) const
{
	if (!enemy || !target) return false;

	MapPosition e = enemy->GetPosition();
	MapPosition t = target->GetPosition();

	int dx = abs(e.x - t.x);
	int dz = abs(e.z - t.z);

	if (dx + dz != 1)
		return false;

	int tile = GetTile(mapData, t.x, t.z, mapW, mapH);
	return tile == static_cast<int>(EMapTile::Player);
}

bool EnemyAI::CanMove(UnitComponent* enemy) const
{
	// 今回は「移動可能か？」だけなので常にtrue
	// 実際の移動可否（壁など）はGameSystem側で弾く想定
	return enemy != nullptr;
}

UnitAction EnemyAI::MakeShadowKillAction(
	UnitComponent* enemy,
	UnitComponent* target,
	MapPosition sunDir
) const
{
	UnitAction action;
	action.type = UnitActionType::Move;
	action.targetGrid = CalcShadowPosition(target, sunDir);
	return action;
}

MapPosition EnemyAI::CalcShadowPosition(
	UnitComponent* target,
	MapPosition sunDir
) const
{
	MapPosition tpos = target->GetPosition();

	// 太陽方向と逆に影が伸びる
	MapPosition shadow;
	shadow.x = tpos.x - sunDir.x;
	shadow.z = tpos.z - sunDir.z;

	return shadow;
}

UnitAction EnemyAI::MakeAttackAction(
	UnitComponent* enemy,
	UnitComponent* target
) const
{
	UnitAction action;
	action.type = UnitActionType::Attack;
	action.targetGrid = target->GetPosition();
	return action;
}

UnitAction EnemyAI::MakeMoveCloserAction(
	UnitComponent* enemy,
	UnitComponent* target,
	const std::vector<UnitComponent*>& allUnits,
	const int* const* mapData,
	int mapW,
	int mapH
) const
{
	UnitAction action;
	action.type = UnitActionType::Move;
	action.targetGrid = DecideMoveCloser(enemy, target, allUnits, mapData, mapW, mapH);
	return action;
}

MapPosition EnemyAI::DecideMoveCloser(
    UnitComponent* enemy,
    UnitComponent* target,
	const std::vector<UnitComponent*>& allUnits,
	const int* const* mapData,
	int mapW,
	int mapH
) const
{
	if (!enemy || !target) return enemy->GetPosition();

	MapPosition epos = enemy->GetPosition();
	MapPosition tpos = target->GetPosition();

	MapPosition best = epos;
	int bestDist = INT_MAX;

	// 探索範囲を 5x5 に拡張（dx/dz = -2..2）
	for (int dz = -3; dz <= 3; dz++)
	{
		for (int dx = -3; dx <= 3; dx++)
		{
			MapPosition p{ epos.x + dx, epos.z + dz };

			// マップ外チェック
			int tile = GetTile(mapData, p.x, p.z, mapW, mapH);
			if (!IsWalkableTile(tile)) continue;

			// 他ユニットがいるマスは除外
			bool occupied = false;
			for (auto* u : allUnits)
			{
				if (!u) continue;
				if (u->GetPosition() == p)
				{
					occupied = true;
					break;
				}
			}
			if (occupied) continue;

			// Player との距離で評価
			int dist = abs(p.x - tpos.x) + abs(p.z - tpos.z);

			if (dist < bestDist)
			{
				// 移動可能範囲（3x3）の中なら候補として採用
				if (abs(dx) <= 1 && abs(dz) <= 1)
				{
					bestDist = dist;
					best = p;
				}
			}
		}
	}

	// 移動できない場合は epos のまま
	return best;
}
