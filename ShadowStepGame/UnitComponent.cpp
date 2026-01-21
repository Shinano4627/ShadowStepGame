#include "UnitComponent.h"
#include "IOManager.h"
#include <iostream>
// ===================================================================
// コンストラクタ
// ===================================================================
UnitComponent::UnitComponent()
{
	//初期化
	m_status.camp = UnitCamp::UnitPlayer;
	m_status.model = UnitModel::UnitNormal;
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
// 移動
// ===================================================================
void UnitComponent::Move(const MapPosition& target)
{
	//行動済みなら何もしない
	if (!CanAct())
		return;

	//座標更新
	m_gridPos = target;

	//行動済みにする
	m_hasActed = true;

#ifdef _DEBUG
	std::cout
		<< "[Unit Move] ("
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
void UnitComponent::Place(const MapPosition& target)
{
	if (!CanAct())
		return;

	//マップ座標設定
	m_gridPos = target;

	m_hasActed = true;

#ifdef _DEBUG
	std::cout
		<< "[Place] pos = ("
		<< target.posX << ","
		<< target.posZ << ")"
		<< std::endl;
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