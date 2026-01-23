#include "UnitComponent.h"
#include "IOManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameSystemComponent.h"
#include "UnitSystemComponent.h"
#include <iostream>

std::vector<UnitComponent*> UnitComponent::s_allUnits;

// ===================================================================
// コンストラクタ
// ===================================================================
UnitComponent::UnitComponent()
{
	//初期ステータス
	m_status.camp = UnitCamp::UnitPlayer;
	m_status.model = UnitModel::UnitAttacker;
	m_status.hp = 5;

	//ユニット管理リスト登録
	s_allUnits.push_back(this);
}
// ===================================================================
// デストラクタ
// ===================================================================
UnitComponent::~UnitComponent()
{
	s_allUnits.erase(
		std::remove(s_allUnits.begin(), s_allUnits.end(), this),
		s_allUnits.end()
		);
}

// ===================================================================
// 更新
// ===================================================================
void UnitComponent::Update()
{
	//プレイヤーのみ入力受付
	if (m_status.camp != UnitCamp::UnitPlayer)
	{
		return;
	}
	m_input.Update();

	// ===================================================================
    // デバッグ：ターンリセット
    // ===================================================================
	if (m_input.GetKeyTrigger(VK_RETURN))
	{
		ResetTurn();
		return;
	}

	//行動不可なら何もしない
	if (!CanAct())
		return;

	// ===================================================================
	// 状態別処理
	// ===================================================================
	if (m_isPlacing)
	{
		UpdatePlacing();
		return;
	}

	if (m_isMoveSelecting)
	{
		UpdateMoveSelecting();
		return;
	}

	if (m_isAttacking)
	{
		UpdateAttacking();
		return;
	}


	// ===================================================================
    // 通常状態(コマンド入力待ち)
    // ===================================================================
	//--Mキーで移動--//
	if (m_input.GetKeyTrigger(VK_M))
	{
		BeginMove();
		return;
	}

	//--Pキーで配置--//
	if (m_input.GetKeyTrigger(VK_P))
	{
		BeginPlace();
		return;
	}

	//--Kキーで攻撃--//
	if (m_input.GetKeyTrigger(VK_K))
	{
		BeginAttack();
		return;
	}
}

// ===================================================================
// 行動可否判定
// ===================================================================
bool UnitComponent::CanAct() const
{
	//そのターンにまだ行動していない
	return !m_hasActed && IsAlive() && !m_isDown;
}


// ===================================================================
// 移動処理
// ===================================================================
void UnitComponent::BeginMove()
{
	if (!CanAct())
		return;

	m_isMoveSelecting = true;
	m_moveTarget = m_gridPos;

#ifdef _DEBUG
	std::cout << "[BeginMove]" << std::endl;
#endif
}

void UnitComponent::UpdateMoveSelecting()
{
	//キャンセルキー(C)で移動選択終了
	if (m_input.GetKeyTrigger(VK_C))
	{
		m_isMoveSelecting = false;
#ifdef _DEBUG
		std::cout << "[MoveMode Cancelled]" << std::endl;
#endif
		return;
	}

	bool moved = false;

	//カーソル移動(矢印キー)
	if (m_input.GetKeyTrigger(VK_UP))
	{
		m_moveTarget.posZ += 1;
		moved = true;
	}
	else if (m_input.GetKeyTrigger(VK_DOWN))
	{
		m_moveTarget.posZ -= 1;
		moved = true;
	}
	else if (m_input.GetKeyTrigger(VK_LEFT))
	{
		m_moveTarget.posX -= 1;
		moved = true;
	}
	else if (m_input.GetKeyTrigger(VK_RIGHT))
	{
		m_moveTarget.posX += 1;
		moved = true;
	}

	if (moved)
	{
#ifdef _DEBUG
		std::cout << "[Move Target] ("
			<< m_moveTarget.posX << "," << m_moveTarget.posZ << ")" << std::endl;
#endif
	}

	//スペースで移動確定
	if (m_input.GetKeyTrigger(VK_SPACE))
	{
		// 移動先に誰かいたら移動不可
		if (IsOccupied(m_moveTarget))
		{
#ifdef _DEBUG
			std::cout << "[Move Failed] Occupied ("
				<< m_moveTarget.posX << ","
				<< m_moveTarget.posZ << ")" << std::endl;
#endif
			return;
		}

		int dx = abs(m_moveTarget.posX - m_gridPos.posX);
		int dz = abs(m_moveTarget.posZ - m_gridPos.posZ);

		//上下左右1マスのみ移動可能
		if (dx + dz == 1)
		{
			Move(m_moveTarget);
			m_isMoveSelecting = false;
		}
		else
		{
#ifdef _DEBUG
			std::cout << "[Cannot move there]" << std::endl;
#endif
		}
	}
}
void UnitComponent::Move(const MapPosition& target)
{
	//行動済みなら何もしない
	if (!CanAct())
		return;

	std::cout
		<< "[Move Before]("
		<< m_gridPos.posX << ","
		<< m_gridPos.posZ << ")"
		<< std::endl;

	//座標更新
	m_gridPos = target;

	//グリッド→ワールド変換
	Vector3 worldPos = GridToWorld(m_gridPos);
	GetOwner()->GetTransform().SetPosition(worldPos);

	GetOwner()->GetTransform().SetRotation(Vector3(0.0f, 0.0f, 0.0f));

	//行動済みにする
	m_hasActed = true;

#ifdef _DEBUG
	std::cout
		<< "[Move after] ("
		<< target.posX << ","
		<< target.posZ << ")"
		<< std::endl;
#endif
}
// ===================================================================
// 影移動
// ===================================================================
void UnitComponent::ShadowMove(const MapPosition& target)
{
	if (!CanAct())
		return;

	m_gridPos = target;

	m_hasActed = true;

#ifdef _DEBUG
	std::cout
		<< "[ShadowMove] pos = ("
		<< target.posX << ","
		<< target.posZ << ")"
		<< std::endl;
#endif
}

// ===================================================================
// 影を踏まれる
// ===================================================================
void UnitComponent::Kill()
{
	m_status.hp = 0;
	m_isDown = false;
	m_hasActed = true;

#ifdef _DEBUG
	std::cout << "[Kill]" << std::endl;
#endif
}

// ===================================================================
// 攻撃
// ===================================================================
void UnitComponent::Attack(UnitComponent* target)
{
	if (!CanAct()||!target) return;


	//仮ダメージ
	const int damage = 5;

#ifdef _DEBUG
	std::cout
		<< "[Attack] From ID:" << GetUnitId()
		<< "To ID:" << target->GetUnitId()
		<< std::endl;
#endif

	target->TakeDamage(damage);
	m_hasActed = true;
}

void UnitComponent::BeginAttack()
{
	if (!CanAct()) return;

	m_isAttacking = true;
	m_attackTarget = nullptr;

	m_attackCursorPos = m_gridPos;

#ifdef _DEBUG
	std::cout << "[AttackMode Begin] Cursor("
		<< m_attackCursorPos.posX << ","
		<< m_attackCursorPos.posZ << ")" << std::endl;
#endif
}
//攻撃モード更新
void UnitComponent::UpdateAttacking()
{
	//キャンセル
	if (m_input.GetKeyTrigger(VK_C))
	{
		m_isAttacking = false;
#ifdef _DEBUG
		std::cout << "[AttackMode Cancelled]" << std::endl;
#endif
		return;
	}

	bool attacked = false;

	if (m_input.GetKeyTrigger(VK_UP))    m_attackCursorPos.posZ += 1,attacked = true;
	else if (m_input.GetKeyTrigger(VK_DOWN)) m_attackCursorPos.posZ -= 1, attacked = true;
	else if (m_input.GetKeyTrigger(VK_LEFT)) m_attackCursorPos.posX -= 1, attacked = true;
	else if (m_input.GetKeyTrigger(VK_RIGHT)) m_attackCursorPos.posX += 1, attacked = true;

	if (attacked) {
		//敵チェック
		UnitComponent* enemy = FindEnemyAt(m_attackCursorPos);
		if (enemy)
		{
			m_attackTarget = enemy;
#ifdef _DEBUG
			std::cout << "[Attack Target Found] ("
				<< m_attackCursorPos.posX << "," << m_attackCursorPos.posZ << ")" << std::endl;
#endif
		}
		else
		{
			m_attackTarget = nullptr;
#ifdef _DEBUG
			std::cout << "[No Enemy At]("
				<< m_attackCursorPos.posX << "," << m_attackCursorPos.posZ << ")" << std::endl;
#endif
		}
	}

	//スペースで攻撃確定
	if (m_input.GetKeyTrigger(VK_SPACE) && m_attackTarget != nullptr)
	{
		Attack(m_attackTarget);
		m_isAttacking = false;
	}
}


// ===================================================================
// 被ダメ
// ===================================================================
void UnitComponent::TakeDamage(int damage)
{
	if (!IsAlive())
		return;

	m_status.hp -= damage;

	if (m_status.hp <= 0)
	{
		m_status.hp = 0;
		Down();
	}

#ifdef _DEBUG
	std::cout
		<< "[TakeDamage]HP = "
		<< m_status.hp
		<< std::endl;
#endif 

}
// ===================================================================
// ダウン状態
// ===================================================================
void UnitComponent::Down()
{
	if (m_isDown)
		return;

	m_isDown = true;
	m_hasActed = true;

#ifdef _DEBUG
	std::cout << "[Down]" << std::endl;
#endif
}
// ===================================================================
// 配置
// ===================================================================


void UnitComponent::BeginPlace()
{
	if (!CanAct())
		return;

	m_isPlacing = true;

	// 初期候補：自分の1マス上
	m_placeTarget = m_gridPos;
	m_placeTarget.posZ += 1;

#ifdef _DEBUG
	std::cout << "[PlaceMode Begin]" << std::endl;
	std::cout << "Self   : (" << m_gridPos.posX << "," << m_gridPos.posZ << ")" << std::endl;
	std::cout << "Target : (" << m_placeTarget.posX << "," << m_placeTarget.posZ << ")" << std::endl;
#endif
}

void UnitComponent::UpdatePlacing()
{
	//キャンセルキー(C)で配置モード終了
	if (m_input.GetKeyTrigger(VK_C))
	{
		m_isPlacing = false;
#ifdef _DEBUG
		std::cout << "[PlaceMode Cancelled]" << std::endl;
#endif
		return;
	}

	bool placed = false;

	if (m_input.GetKeyTrigger(VK_UP))        m_placeTarget = { m_gridPos.posX,m_gridPos.posZ + 1 }, placed = true;
	else if (m_input.GetKeyTrigger(VK_DOWN)) m_placeTarget = { m_gridPos.posX,m_gridPos.posZ - 1 },placed = true;
	else if (m_input.GetKeyTrigger(VK_LEFT)) m_placeTarget = { m_gridPos.posX-1,m_gridPos.posZ  }, placed = true;
	else if (m_input.GetKeyTrigger(VK_RIGHT))m_placeTarget = { m_gridPos.posX+1,m_gridPos.posZ  }, placed = true;

	if (placed) {
		//自分の位置には置けない
		if (m_placeTarget.posX == m_gridPos.posX && m_placeTarget.posZ == m_gridPos.posZ)
		{
#ifdef _DEBUG
			std::cout << "Invalid Place Target] ("
				<< m_placeTarget.posX << "," << m_placeTarget.posZ << ")" << std::endl;
#endif
		}
		else
		{
#ifdef _DEBUG
			std::cout << "[Place Target]("
				<< m_placeTarget.posX << "," << m_placeTarget.posZ << ")" << std::endl;
#endif
		}
	}

	if (m_input.GetKeyTrigger(VK_SPACE))
	{
		//自分のマスや斜めは置けないルール
		int dx = abs(m_placeTarget.posX - m_gridPos.posX);
		int dz = abs(m_placeTarget.posZ - m_gridPos.posZ);

		if (dx + dz == 1)
		{
			PlaceObstacle();
			m_isPlacing = false;
			m_hasActed = true;
		}
		else
		{
#ifdef _DEBUG
			std::cout << "Cannot place here]" << std::endl;
#endif
		}
	}
}
void UnitComponent::TryPlaceObstacle()
{
	int dx = abs(m_placeTarget.posX - m_gridPos.posX);
	int dz = abs(m_placeTarget.posZ - m_gridPos.posZ);

	//自分のマスは禁止
	if (dx == 0 && dz == 0)
		return;

	//上下左右1マスのみ
	if (dx + dz != 1)
		return;

	PlaceObstacle();
	m_isPlacing = false;
	m_hasActed = true;
}

// ===================================================================
// オブジェクト生成
// ===================================================================
void UnitComponent::PlaceObstacle()
{
	//TODO:オブジェクト生成
	//シーン管理クラスで行う？
	//今はログのみ(Self:自コマ位置　Target:設置位置)

#ifdef _DEBUG
	std::cout
		<< "[PlaceObstacle]" << std::endl
		<< "Self :(" << m_gridPos.posX << "," << m_gridPos.posZ << ")" << std::endl
		<< "Target :(" << m_placeTarget.posX << "," << m_placeTarget.posZ << ")" << std::endl;
#endif
}

// ===================================================================
// 壁破壊
// ===================================================================
void UnitComponent::BreakWall(const MapPosition& target)
{
	if (!CanAct()) return;

	if (m_status.model != UnitModel::UnitGiant)
		return;

	m_hasActed = true;

#ifdef _DEBUG
	std::cout
		<< "[BreakWall] pos = ("
		<< target.posX << ","
		<< target.posZ << ")"
		<< std::endl;
#endif
}

// ===================================================================
// 状態取得
// ===================================================================
bool UnitComponent::IsAlive() const

{
	return m_status.hp > 0;
}

bool UnitComponent::IsDown()const
{
	return m_isDown;
}


// ===================================================================
// ターンリセット(デバッグ用)
// ===================================================================
void UnitComponent::ResetTurn()
{
	m_hasActed = false;

	m_isMoveSelecting = false;
	m_isPlacing = false;
	m_isAttacking = false;

#ifdef _DEBUG
	std::cout<< "[Turn Reset]" << std::endl;
#endif
}

Vector3 UnitComponent::GridToWorld(const MapPosition& grid) const
{
	const float CELL_SIZE = 1.0f;

	return Vector3(
		grid.posX * CELL_SIZE,
		0.0f,
		grid.posZ * CELL_SIZE
	);


}
// ===================================================================
// 敵ユニット検索(簡易)
// ===================================================================
UnitComponent* UnitComponent::FindEnemyAt(const MapPosition& pos)
{
#ifdef _DEBUG
	std::cout << "[FindEnemyAt] Check Pos("
		<< pos.posX << "," << pos.posZ << ")\n";
	std::cout << " AllUnits Count:" << s_allUnits.size() << std::endl;
#endif

	for (UnitComponent* unit : s_allUnits)
	{
		if (!unit) continue;

#ifdef _DEBUG
		std::cout << "  Unit ID:" << unit->GetUnitId()
			<< " Pos(" << unit->GetGridPos().posX
			<< "," << unit->GetGridPos().posZ << ")"
			<< " Camp:" << static_cast<int>(unit->GetCamp())
			<< std::endl;
#endif

		if (unit == this) continue;
		if (!unit->IsAlive()) continue;
		if (unit->GetCamp() == m_status.camp) continue;

		const auto& uPos = unit->GetGridPos();
		if (uPos.posX == pos.posX && uPos.posZ == pos.posZ)
			return unit;
	}
	return nullptr;
}

// ===================================================================
// 指定グリッドが他ユニットに占有されているか
// ===================================================================
bool UnitComponent::IsOccupied(const MapPosition& pos) const
{
	for (UnitComponent* unit : s_allUnits)
	{
		if (!unit->IsAlive()) continue;

		const auto& uPos = unit->GetGridPos();
		if (uPos.posX == pos.posX &&
			uPos.posZ == pos.posZ)
		{
			return true;
		}
	}
	return false;
}
