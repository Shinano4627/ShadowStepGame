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

void UnitSystemComponent::RegisterUnit(UnitComponent* unit)
{
	if (!unit) return;
	// UnitComponentの登録
	if (std::find(m_UnitList.begin(), m_UnitList.end(), unit) == m_UnitList.end())
	{
		m_UnitList.push_back(unit);
	}
}

void UnitSystemComponent::UnRegisterUnit(UnitComponent* unit)
{
	m_UnitList.erase(
		std::remove(m_UnitList.begin(), m_UnitList.end(), unit),
		m_UnitList.end());
}

const std::vector<UnitComponent*> UnitSystemComponent::GetAllUnits() const
{
	return m_UnitList;
}

std::vector<UnitComponent*> UnitSystemComponent::GetAliveUnits() const
{
	std::vector<UnitComponent*> result;

	for (auto* unit : m_UnitList)
	{
		
			result.push_back(unit);
		
	}
	return result;
}

std::vector<UnitComponent*> UnitSystemComponent::GetUnitsSortedBySpeed() const
{
	auto units = GetAliveUnits();

	std::sort(units.begin(), units.end(),
		[](UnitComponent* a, UnitComponent* b)
		{
			return a->GetSpeed() > b->GetSpeed();
		});

	return units;
}


UnitComponent* UnitSystemComponent::FindUnit(int id)
{
	for (auto& u : m_UnitList)
	{
		if (u->GetId() == id)
			return u;
	}
	return nullptr;
}

UnitComponent* UnitSystemComponent::FindUnitAtPosition(const MapPosition& pos) const
{
	for (auto* unit : m_UnitList)
	{
		if (!unit) continue;
		
		MapPosition uPos = unit->GetPosition();
		if (uPos.x == pos.x && uPos.z == pos.z)
		{
			return unit;
		}
	}
	return nullptr;
}

bool UnitSystemComponent::IsPlayerAllDead() const
{
	for (auto& u : m_UnitList)
		if (u->GetType() == UnitType::Player)
			return false;
	return true;
}

bool UnitSystemComponent::IsEnemyAllDead() const
{
	for (auto& u : m_UnitList)
		if (u->GetType() == UnitType::Enemy)
			return false;
	return true;
}