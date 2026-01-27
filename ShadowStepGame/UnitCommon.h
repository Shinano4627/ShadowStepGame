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

	// コンストラクタ
	MapPosition() = default;
	MapPosition(int _x, int _z)
		: x(_x)
		, z(_z)
	{}

	// オペレーターオーバーロード
	MapPosition& operator=(const MapPosition& pos)
	{
		x = pos.x;
		z = pos.z;
		return *this;
	}
	MapPosition operator+(const MapPosition& pos) const
	{
		return MapPosition{ x + pos.x,z + pos.z };
	}
	MapPosition& operator+=(const MapPosition& pos)
	{
		x += pos.x;
		z += pos.z;
		return *this;
	}
	MapPosition operator-(const MapPosition& pos) const
	{
		return MapPosition{ x - pos.x,z - pos.z };
	}
	MapPosition& operator-=(const MapPosition& pos)
	{
		x -= pos.x;
		z -= pos.z;
		return *this;
	}
	MapPosition operator*(const MapPosition& pos) const
	{
		return MapPosition{ x * pos.x, z * pos.z };
	}
	MapPosition& operator*=(const MapPosition& pos)
	{
		x *= pos.x;
		z *= pos.z;
		return *this;
	}
	MapPosition operator/(const MapPosition& pos) const
	{
		return MapPosition{ x / pos.x, z / pos.z };
	}
	MapPosition& operator/=(const MapPosition& pos)
	{
		x /= pos.x;
		z /= pos.z;
		return *this;
	}
	MapPosition operator+(const int& n) const
	{
		return MapPosition{ x + n, z + n };
	}
	MapPosition& operator+=(const int& n)
	{
		x += n;
		z += n;
		return *this;
	}
	MapPosition operator-(const int& n)const
	{
		return MapPosition{ x - n, z - n };
	}
	MapPosition& operator-=(const int& n)
	{
		x -= n;
		z -= n;
		return *this;
	}
	MapPosition operator*(const int& n)const
	{
		return MapPosition{ x * n, z * n };
	}
	MapPosition& operator*=(const int& n)
	{
		x *= n;
		z *= n;
		return *this;
	}
	MapPosition operator/(const int& n)const
	{
		return MapPosition{ x / n, z / n };
	}
	MapPosition& operator/=(const int& n)
	{
		x /= n;
		z /= n;
		return *this;
	}
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

