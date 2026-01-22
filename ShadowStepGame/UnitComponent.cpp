#include "UnitComponent.h"
#include "IOManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameSystemComponent.h"
#include "UnitSystemComponent.h"
#include <iostream>


// ===================================================================
// コンストラクタ
// ===================================================================
UnitComponent::UnitComponent()
{
	//初期化
	m_status.camp = UnitCamp::UnitPlayer;
	m_status.model = UnitModel::UnitAttacker;
	m_status.hp = 5;
}
// ===================================================================
// デストラクタ
// ===================================================================
UnitComponent::~UnitComponent()
{

}

// ===================================================================
// 更新
// ===================================================================
void UnitComponent::Update()
{
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

	//--Aキーで攻撃--//
	if (m_input.GetKeyTrigger(VK_A))
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

	//移動候補が未初期化なら自分の位置に設定
	if (m_moveTarget.posX == 0 && m_moveTarget.posZ == 0)
		m_moveTarget = m_gridPos;

	//カーソル移動(矢印キー)
	if (m_input.GetKeyTrigger(VK_UP))
	{
		m_moveTarget.posZ += 1;
	}
	else if (m_input.GetKeyTrigger(VK_DOWN))
	{
		m_moveTarget.posZ -= 1;
	}
	else if (m_input.GetKeyTrigger(VK_LEFT))
	{
		m_moveTarget.posX -= 1;
	}
	else if (m_input.GetKeyTrigger(VK_RIGHT))
	{
		m_moveTarget.posX += 1;
	}
#ifdef _DEBUG
	std::cout << "[Move Target] ("
		<< m_moveTarget.posX << "," << m_moveTarget.posZ << ")" << std::endl;
#endif

	//スペースで移動確定
	if (m_input.GetKeyTrigger(VK_SPACE))
	{
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
	if (!CanAct()) return;

	if (target == nullptr)
		return;

	//仮ダメージ
	const int damage = 5;

	target->TakeDamage(damage);

	m_hasActed = true;

#ifdef _DEBUG
	std::cout
		<< "[Attack] tagetHP = "
		<< target->GetHP()
		<< std::endl;
#endif
}

void UnitComponent::BeginAttack()
{
	if (!CanAct()) return;

	m_isAttacking = true;

	//初期候補は自分の右隣(隣接マスに敵がいればそこにせってい)
	//TODO:
	//敵ユニット管理クラスから隣接マスの敵を取得

#ifdef _DEBUG
	std::cout << "[AttackMode Begin]" << std::endl;
#endif
}
//攻撃モード更新
void UnitComponent::UpdateAttacking()
{
	if (m_input.GetKeyTrigger(VK_C))
	{
		m_isAttacking = false;
#ifdef _DEBUG
		std::cout << "[AttackMode Cancelled]" << std::endl;
#endif
		return;
	}

	//矢印キーで隣接マスの敵にカーソル移動
	MapPosition candidatePos = m_gridPos;

	if (m_input.GetKeyTrigger(VK_UP))    candidatePos.posZ += 1;
	else if (m_input.GetKeyTrigger(VK_DOWN)) candidatePos.posZ -= 1;
	else if (m_input.GetKeyTrigger(VK_LEFT)) candidatePos.posX -= 1;
	else if (m_input.GetKeyTrigger(VK_RIGHT)) candidatePos.posX += 1;

	//UnitComponent* target = GetEnemyAtPos(candidatePos);
	/*if (target != nullptr)
		m_attackTarget = target;*/

#ifdef _DEBUG
	if (m_attackTarget)
		std::cout << "[Attack Target] (" << m_attackTarget->GetGridPos().posX
		<< "," << m_attackTarget->GetGridPos().posZ << ")" << std::endl;
#endif

	//スペースで攻撃確定
	if (m_input.GetKeyTrigger(VK_SPACE) && m_attackTarget != nullptr)
	{
		Attack(m_attackTarget);
		m_isAttacking = false;
	}
}

//敵取得簡易関数
//UnitComponent* UnitComponent::GetEnemyAtPos(const MapPosition& pos)
//{
//	for (UnitComponent* enemy : g_EnemyList) // 例えば敵のリストを管理しているとする
//	{
//		if (!enemy->IsAlive()) continue;
//		if (enemy->GetGridPos().posX == pos.posX && enemy->GetGridPos().posZ == pos.posZ)
//			return enemy;
//	}
//	return nullptr;
//}

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
	if (m_input.GetKeyTrigger(VK_UP))        m_placeTarget = { m_gridPos.posX,m_gridPos.posZ + 1 };
	else if (m_input.GetKeyTrigger(VK_DOWN)) m_placeTarget = { m_gridPos.posX,m_gridPos.posZ - 1 };
	else if (m_input.GetKeyTrigger(VK_LEFT)) m_placeTarget = { m_gridPos.posX-1,m_gridPos.posZ  };
	else if (m_input.GetKeyTrigger(VK_RIGHT))m_placeTarget = { m_gridPos.posX+1,m_gridPos.posZ  };

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
