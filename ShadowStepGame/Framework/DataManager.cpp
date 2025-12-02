#include "DataManager.h"

#include <fstream>
//#include <nlohmann/json.hpp>	// json操作用ライブラリ
#include <filesystem>

using namespace std;
//using json = nlohmann::json;

// データフォルダパス
string DataDirPath;
char path[MAX_PATH];  // パスを格納するバッファ

//設定ファイル
const string SettingFile = "/Setting.json";

#pragma region プロトタイプ宣言
//void from_json(const json& j, DataStore& p);
char* get_fullpath(std::string _fileName);
#pragma endregion

//json jsonDB;

/// <summary>
/// 初期化
/// </summary>
void DataManager::Init()
{
    // 各種初期データの読み込み
    InitializeSetting();
}

void DataManager::UnInit()
{
}

void DataManager::Update()
{
}

char* get_fullpath(std::string _fileName)
{
    memset(path, '\0', sizeof(path));
    memcpy(path, DataDirPath.c_str(), MAX_PATH);
    strcat_s(path, _fileName.c_str());

    return path;
}

// 初期値をデータ管理から取得
void DataManager::InitializeSetting()
{
}
