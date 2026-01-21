// ===================================================================
// UnitSystemComponent.cpp
// ===================================================================
#include "UnitSystemComponent.h"
#include <algorithm>

void UnitSystemComponent::Update()
{

}

void UnitSystemComponent::Uninit()
{

}

void UnitSystemComponent::RegisterUnit(const UnitStatus& status)
{
	m_UnitList.push_back(std::make_unique<UnitStatus>(status));
}

void UnitSystemComponent::KillUnit(int id)
{
	m_UnitList.erase(
		std::remove_if(m_UnitList.begin(), m_UnitList.end(),
			[id](const std::unique_ptr<UnitStatus>& u)
			{
				return u->id == id;
			}),
		m_UnitList.end());
}

void UnitSystemComponent::DamageUnit(int id, int damage)
{
	auto* unit = FindUnit(id);
	if (!unit) return;

	unit->hp -= damage;

	if (unit->hp <= 0)
	{
		KillUnit(id);
	}
}

std::vector<UnitStatus*> UnitSystemComponent::GetAllUnits() const
{
	std::vector<UnitStatus*> result;
	result.reserve(m_UnitList.size());

	for (const auto& u : m_UnitList)
	{
		result.push_back(u.get());
	}

	return result; // 値返し（コピーはポインタのみ）
}


std::vector<UnitStatus*> UnitSystemComponent::GetUnitsSortedBySpeed() const
{
	std::vector<UnitStatus*> result;

	for (auto& u : m_UnitList)
	{
		if (!u->isDown)
			result.push_back(u.get());
	}

	std::sort(result.begin(), result.end(),
		[](UnitStatus* a, UnitStatus* b)
		{
			return a->speed > b->speed;
		});

	return result;
}

UnitStatus* UnitSystemComponent::FindUnit(int id)
{
	for (auto& u : m_UnitList)
	{
		if (u->id == id)
			return u.get();
	}
	return nullptr;
}

bool UnitSystemComponent::IsPlayerAllDead() const
{
	for (auto& u : m_UnitList)
		if (u->type == UnitType::Player)
			return false;
	return true;
}

bool UnitSystemComponent::IsEnemyAllDead() const
{
	for (auto& u : m_UnitList)
		if (u->type == UnitType::Enemy)
			return false;
	return true;
}