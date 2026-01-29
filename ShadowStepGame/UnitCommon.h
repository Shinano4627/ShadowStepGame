//=======================================
// UnitCommon.h
//=======================================
#pragma once

enum class UnitType
{
	Player,
	Enemy
};

enum class UnitModel
{
	Attack,
	Place,
	Giant
};

struct MapPosition
{
	int x;
	int z;
};

struct UnitStatus
{
	int id;		// GameObject IDと一致させる
	UnitType type;	// Player / Enemy
	UnitModel model;	// ユニット種別
	MapPosition pos;	// マップ座標
	int hp;
	int speed;
	bool isDown = false;	// 行動不能
};

enum class UnitActionType
{
	None,
	Move,
	Attack,
	Place,
	ShadowMove,
};

struct UnitAction
{
	UnitActionType type = UnitActionType::None;
	MapPosition targetGrid;
};