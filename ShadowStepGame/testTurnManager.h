#pragma once
#include <vector>

class UnitComponent;

class TurnManager
{
public:
	//シングルトン(最小構成用)
	static TurnManager& Get()
	{
		static TurnManager instance;
		return instance;
	}

	//ユニット登録
	void RegisterUnit(UnitComponent* unit);

	//ターン開始
	void StartBattle();

	//毎フレーム更新
	void Update();

	//今のユニットか？
	bool IsCurrentUnit(const UnitComponent* unit) const;

private:
	TurnManager() = default;

	void StartCurrentTurn();
	void NextTurn();

private:
	std::vector<UnitComponent*>m_units;
	int m_currentIndex = -1;
};