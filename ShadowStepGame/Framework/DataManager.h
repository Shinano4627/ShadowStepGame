#pragma once
#include "Application.h"

#include "singleton.h"

class DataManager
{
/// <summary>
/// 変数
/// </summary>
private:

public:

/// <summary>
/// 関数
/// </summary>
public:
	void Init();
	void UnInit();
	void Update();

private:
	void InitializeSetting();
};

// 唯一インスタンス取得マクロ
#define DATA_MANAGER Singleton<DataManager>::GetInstance()

