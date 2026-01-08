#pragma once
#include "Component.h"
#include <vector>

class UnitComponent;

class TurnComponent : public Component
{
private:
    std::vector<UnitComponent*> m_units;  // 全ユニット
    int m_currentIndex = 0;

public:
    TurnComponent() = default;
    ~TurnComponent() = default;

    void Update() override;

    void AddUnit(UnitComponent* unit);
    void StartNextTurn();
};
