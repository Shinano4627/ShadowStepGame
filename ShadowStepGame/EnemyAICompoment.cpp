#include "EnemyAICompoment.h"
#include <cmath>
#include <iostream>

// ===================================================================
// メイン判断関数（フローチャート準拠）
// ===================================================================
UnitAction EnemyAI::DecideAction(
	UnitComponent* enemy,
	const std::vector<UnitComponent*>& playerList,
	const std::vector<UnitComponent*>& enemyList,
	const std::vector<UnitComponent*>& allUnits,
	MapPosition sunDirection,
	const int* const* mapData,
	int mapWidth,
	int mapHeight
)
{
	UnitAction none;
	none.type = UnitActionType::None;
	none.targetGrid = MapPosition(0, 0);

	if (!enemy) return none;

	// ===================================================================
	// ① 周囲10マス以内にプレイヤーがいるか？
	// ===================================================================
	UnitComponent* target = FindNearestPlayerInRange(enemy, playerList);
	if (!target)
	{
		std::cout << "[EnemyAI] 周囲にプレイヤーなし → 何もしない" << std::endl;
		return none;
	}

	MapPosition epos = enemy->GetPosition();
	MapPosition tpos = target->GetPosition();
	int dist = ManhattanDist(epos, tpos);

	std::cout << "[EnemyAI] ターゲット発見 距離=" << dist << std::endl;

	// ===================================================================
	// ② 最近のプレイヤーとの距離は4マス以上か？
	// ===================================================================
	if (dist >= CLOSE_RANGE)
	{
		// 遠い → プレイヤーへ向かって移動
		std::cout << "[EnemyAI] 距離4以上 → プレイヤーへ接近" << std::endl;

		MapPosition moveTo = CalcMoveOneStep(
			epos, tpos, true,
			allUnits, enemy, mapData, mapWidth, mapHeight);

		if (moveTo == epos) return none;

		UnitAction action;
		action.type = UnitActionType::Move;
		action.targetGrid = moveTo;
		return action;
	}

	// ===================================================================
	// ③ 近距離：プレイヤーの影と自分の位置を判断
	// ===================================================================

	// ShadowParam計算
	int shadowLength = 0;
	if (!(sunDirection.x == 0 && sunDirection.z == 0))
	{
		int sunDist = abs(sunDirection.x) + abs(sunDirection.z);
		shadowLength = std::clamp(sunDist / 2, 2, 8);
	}

	// 影の位置を計算
	MapPosition shadowPos = CalcShadowPosition(target, sunDirection, shadowLength);

	// 影が有効か（真上でない＆マップ内で歩行可能）
	bool shadowValid = (shadowLength > 0);
	if (shadowValid)
	{
		int tile = GetTile(mapData, shadowPos.x, shadowPos.z, mapWidth, mapHeight);
		if (!IsWalkableTile(tile))
			shadowValid = false;
		// 影の位置が自分の現在地と同じなら対象外
		if (shadowPos == epos)
			shadowValid = false;
	}

	if (shadowValid)
	{
		// 影が自分と同じ側にあるか判断
		bool sameSide = IsShadowOnSameSide(enemy, target, sunDirection);

		if (sameSide)
		{
			// ===================================================================
			// ④ 同じ側 → プレイヤーの影へ移動
			// ===================================================================
			std::cout << "[EnemyAI] 影が同じ側 → 影へ移動" << std::endl;

			MapPosition moveTo = CalcMoveOneStep(
				epos, shadowPos, true,
				allUnits, enemy, mapData, mapWidth, mapHeight);

			if (!(moveTo == epos))
			{
				UnitAction action;
				action.type = UnitActionType::Move;
				action.targetGrid = moveTo;
				return action;
			}
			// 移動できなければ攻撃判定へフォールスルー
		}
		// 同じ側でない → 攻撃判定へ
	}

	// ===================================================================
	// ⑤ プレイヤーを攻撃できるか？（XまたはZが等しく隣接）
	// ===================================================================
	if (CanAttack(enemy, target))
	{
		std::cout << "[EnemyAI] 攻撃可能 → 攻撃" << std::endl;

		UnitAction action;
		target->SetDown(2);

		// ターゲット方向に回転する
		Transform& transform = enemy->GetOwner()->GetTransform();
		Vector3 targetPos = transform.GetPosition();
		targetPos.x = tpos.x * 5.0f;
		targetPos.z = tpos.z * 5.0f;
		enemy->RotateForTarget(targetPos);

		action.type = UnitActionType::Attack;
		action.targetGrid = tpos;
		return action;
	}

	// ===================================================================
	// ⑥ 攻撃不可 → プレイヤーから遠ざかる
	// ===================================================================
	std::cout << "[EnemyAI] 攻撃不可 → 遠ざかる" << std::endl;

	MapPosition moveTo = CalcMoveOneStep(
		epos, tpos, false,  // toward=false → 遠ざかる
		allUnits, enemy, mapData, mapWidth, mapHeight);

	if (moveTo == epos) return none;

	UnitAction action;
	action.type = UnitActionType::Move;
	action.targetGrid = moveTo;
	return action;
}

// ===================================================================
// ① 周囲10マス以内の最近プレイヤーを探す
// ===================================================================
UnitComponent* EnemyAI::FindNearestPlayerInRange(
	UnitComponent* enemy,
	const std::vector<UnitComponent*>& playerList
) const
{
	if (!enemy) return nullptr;

	MapPosition epos = enemy->GetPosition();
	UnitComponent* best = nullptr;
	int bestDist = INT_MAX;

	for (auto* p : playerList)
	{
		if (!p) continue;
		if (p->IsDown()) continue;

		int dist = ManhattanDist(epos, p->GetPosition());

		if (dist <= DETECTION_RANGE && dist < bestDist)
		{
			bestDist = dist;
			best = p;
		}
	}
	return best;
}

// ===================================================================
// ② 影が自分と同じ側にあるか
//    影は上下(Z方向)ならZだけ、左右(X方向)ならXだけで判断
// ===================================================================
bool EnemyAI::IsShadowOnSameSide(
	UnitComponent* enemy,
	UnitComponent* target,
	MapPosition sunDirection
) const
{
	if (!enemy || !target) return false;

	MapPosition epos = enemy->GetPosition();
	MapPosition tpos = target->GetPosition();

	// 太陽がZ方向（上下）に動く → 影はZ方向に伸びる → Zだけで判断
	if (sunDirection.z != 0 && sunDirection.x == 0)
	{
		// 影の方向：光源の逆
		int shadowDirZ = (sunDirection.z > 0) ? -1 : 1;
		// 敵がプレイヤーから見て影と同じZ方向にいるか
		int enemySideZ = epos.z - tpos.z;
		return (enemySideZ * shadowDirZ) > 0;
	}

	// 太陽がX方向（左右）に動く → 影はX方向に伸びる → Xだけで判断
	if (sunDirection.x != 0 && sunDirection.z == 0)
	{
		int shadowDirX = (sunDirection.x > 0) ? -1 : 1;
		int enemySideX = epos.x - tpos.x;
		return (enemySideX * shadowDirX) > 0;
	}

	// 斜めの場合：両軸で判定（どちらも同じ側ならtrue）
	if (sunDirection.x != 0 && sunDirection.z != 0)
	{
		int shadowDirX = (sunDirection.x > 0) ? -1 : 1;
		int shadowDirZ = (sunDirection.z > 0) ? -1 : 1;
		int enemySideX = epos.x - tpos.x;
		int enemySideZ = epos.z - tpos.z;

		bool sameX = (enemySideX * shadowDirX) > 0;
		bool sameZ = (enemySideZ * shadowDirZ) > 0;
		return sameX || sameZ;
	}

	// 真上（影なし）
	return false;
}

// ===================================================================
// ③ 攻撃判定：XまたはZが等しく隣接（マンハッタン距離1）
// ===================================================================
bool EnemyAI::CanAttack(
	UnitComponent* enemy,
	UnitComponent* target
) const
{
	if (!enemy || !target) return false;

	MapPosition e = enemy->GetPosition();
	MapPosition t = target->GetPosition();

	int dx = abs(e.x - t.x);
	int dz = abs(e.z - t.z);

	// XまたはZが等しく、距離1（十字方向に隣接）
	return (dx + dz == 1);
}

// ===================================================================
// 移動先計算：差が大きい軸方向に1マス
// toward=true: 対象に近づく  toward=false: 対象から遠ざかる
// ===================================================================
MapPosition EnemyAI::CalcMoveOneStep(
	MapPosition from,
	MapPosition to,
	bool toward,
	const std::vector<UnitComponent*>& allUnits,
	UnitComponent* self,
	const int* const* mapData,
	int mapW, int mapH
) const
{
	int dx = to.x - from.x;
	int dz = to.z - from.z;

	if (!toward)
	{
		dx = -dx;
		dz = -dz;
	}

	// 方向を正規化（-1, 0, 1）
	int signX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
	int signZ = (dz > 0) ? 1 : (dz < 0) ? -1 : 0;

	// ★移動候補を優先順に（2マスから1マスへフォールバック）
	MapPosition candidates[4];
	int candidateCount = 0;

	if (abs(dx) >= abs(dz))
	{
		// X軸優先
		if (signX != 0) candidates[candidateCount++] = MapPosition(from.x + signX * 2, from.z);           // X方向2マス
		if (signX != 0 && signZ != 0) candidates[candidateCount++] = MapPosition(from.x + signX, from.z + signZ); // 斜め1+1
		if (signX != 0) candidates[candidateCount++] = MapPosition(from.x + signX, from.z);               // X方向1マス
		if (signZ != 0) candidates[candidateCount++] = MapPosition(from.x, from.z + signZ);               // Z方向1マス
	}
	else
	{
		// Z軸優先
		if (signZ != 0) candidates[candidateCount++] = MapPosition(from.x, from.z + signZ * 2);           // Z方向2マス
		if (signX != 0 && signZ != 0) candidates[candidateCount++] = MapPosition(from.x + signX, from.z + signZ); // 斜め1+1
		if (signZ != 0) candidates[candidateCount++] = MapPosition(from.x, from.z + signZ);               // Z方向1マス
		if (signX != 0) candidates[candidateCount++] = MapPosition(from.x + signX, from.z);               // X方向1マス
	}

	// 候補を順に試す
	for (int i = 0; i < candidateCount; i++)
	{
		MapPosition p = candidates[i];

		// マンハッタン距離2以内か
		if (abs(p.x - from.x) + abs(p.z - from.z) > 2) continue;

		// マップタイル確認
		int tile = GetTile(mapData, p.x, p.z, mapW, mapH);
		if (!IsWalkableTile(tile)) continue;

		// ユニット衝突確認
		bool occupied = false;
		for (auto* u : allUnits)
		{
			if (!u || u == self) continue;
			if (u->GetPosition() == p)
			{
				occupied = true;
				break;
			}
		}
		if (occupied) continue;

		return p;
	}

	return from;
}

// ===================================================================
// 影の位置を計算（プレイヤーから光源の逆方向に伸びる）
// ===================================================================
MapPosition EnemyAI::CalcShadowPosition(
	UnitComponent* target,
	MapPosition sunDirection,
	int shadowLength
) const
{
	if (!target) return MapPosition(0, 0);

	MapPosition tpos = target->GetPosition();

	// 影は光源の逆方向に伸びる
	// 方向を正規化（-1, 0, 1）
	int dirX = 0;
	int dirZ = 0;
	if (sunDirection.x != 0) dirX = (sunDirection.x > 0) ? -1 : 1;
	if (sunDirection.z != 0) dirZ = (sunDirection.z > 0) ? -1 : 1;

	MapPosition shadow;
	shadow.x = tpos.x + dirX * shadowLength;
	shadow.z = tpos.z + dirZ * shadowLength;

	return shadow;
}