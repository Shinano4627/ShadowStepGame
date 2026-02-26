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

enum class EMapTile
{
	Empty = 0,  // オブジェクトなし
	Load = 1,  // 道
	Wall = 10,  // 壁
	Tree = 11,  // 木
	Torii = 12,  // 鳥居
	WallBlocken = 13,  // 壊せる壁
	PlayerAttack = 20,  // プレイヤー
	PlayerPlace = 21,  // プレイヤー
	PlayerBig = 22,  // プレイヤー
	EnemyAttack = 30,  // 敵
	EnemyPlace = 31,  // 敵
	EnemyBig = 32,  // 敵
	Shadow = 40,  // 影
	Reserve2 = 50,  // Reserve
	Reserve3 = 60,  // Reserve

	None = 99	// マップ外
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

	bool operator==(const MapPosition& other) const
	{
		return x == other.x && z == other.z;
	}

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
struct MapPositionHash
{
	std::size_t operator()(const MapPosition& pos) const noexcept
	{
		std::size_t hx = std::hash<int>{}(pos.x);
		std::size_t hz = std::hash<int>{}(pos.z);
		return hx ^ (hz << 1); // 定番の合成
	}
};


struct UnitStatus
{
	int id;		// GameObject IDと一致させる
	UnitType type;	// PlayerAttack / EnemyAttack
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

