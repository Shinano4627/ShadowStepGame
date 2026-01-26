#include "MapSystemComponent.h"

#include <fstream>
#include <sstream>
#include <stdio.h>

#include "SimplePlaneRendererComponent.h"
#include "UnitCommon.h"
#include "GameObjectList.h"

using namespace std;

void MapSystemComponent::MakeMap(std::unique_ptr<GameObjectList>& objectList)      // CSVデータ読み込みとマップオブジェクトの作成
{
    int mapZ = 0;
    int mapX = 0;
    int n = 3000;
    ifstream csv_data(m_DataFile, ios::in);

    if (!csv_data.is_open()) {
        cout << "Error: opening file fail" << endl;
        exit(1);
    }
    else {
        cout << "Start Read : " << m_DataFile << endl;

        string line;

        vector<string> words; //文字列ベクトルを宣言する
        string word;
        // ------------データ読み取り-----------------
        istringstream sin;
        // マップの広さを取得
        getline(csv_data, line);
        sin.clear();
        sin.str(line);
        //文字列ストリームsinの文字をコンマ区切り
        getline(sin, word, ',');
        m_MapWidth = stoi(word);
        getline(sin, word, ',');
        m_MapHeight = stoi(word);

        // マップ
        m_MapData = new int* [m_MapHeight]();
        for (int i = 0; i < m_MapHeight; i++)
        {
            m_MapData[i] = new int[m_MapWidth]();
        }

        // 原点を中心に表示されるようにスタート位置を計算
        m_DrawStartPosX = -m_MapWidth * m_SizePiece / 2.f + m_SizePiece / 2.f;
        m_DrawStartPosZ = -m_MapHeight * m_SizePiece / 2.f + m_SizePiece / 2.f;

        // 行ごとにデータを読み込む
        while (getline(csv_data, line)) {
            // vectorおよび文字ストリームをクリアし、前の行 のデータのみを保存します
            words.clear();
            sin.clear();
            sin.str(line);
            //文字列ストリームsinの文字をコンマ区切り文字列配列wordsに配置する
            while (getline(sin, word, ',')) {
                //cout << word << endl;
                words.push_back(word); //セル内のデータを1つずつpushする
            }

            //行に従ったmapの作成を開始
            for (string str : words)
            {
                // トランスフォームデータを渡す
                auto obj = std::make_unique<GameObject>
                    (Vector3(m_DrawStartPosX + mapX * m_SizePiece, 0.2f, m_DrawStartPosZ + mapZ * m_SizePiece), Vector3::Zero, Vector3(m_SizePiece / 2, 1.f, m_SizePiece / 2));
                GameObject* newObject = obj.get();
                newObject->SetID(n);
                newObject->SetName("Map");
                newObject->SetTag("Map");

                int data = stoi(str);
                m_MapData[mapZ][mapX] = data;

                // CSVからのよみとり
                Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
                switch (data)
                {
                case 0:
                    //何もない
                    color = Color(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case 1:
                    //壁
                    color = Color(0.2f, 0.2f, 0.2f, 1.0f);
                    break;
                case 2:
                    //プレーヤー
                    color = Color(0, 0, 1.0f, 1.0f);
                    break;
                case 3:
                    //敵
                    color = Color(1.0f, 0, 0, 1.0f);
                    break;
                case 4:
                    //樹
                    color = Color(0, 1.0f, 0, 1.0f);
                    break;
                case 5:
                    //影
                    color = Color(0.5f, 0.5f, 0.5f, 1.0f);
                    break;
                default:

                    break;
                }
                newObject->AddMeshComponent<SimplePlaneRendererComponent>(color);
                mapX++;
                n++;
                objectList->AddObject(std::move(obj));
            }
            mapX = 0;
            mapZ++;
            //return words;
        }

        csv_data.close();
    }

    // デバッグ出力
    for (int i = 0; i < m_MapHeight; i++)
    {
        for (int j = 0; j < m_MapWidth; j++)
        {
            std::cout << m_MapData[i][j];
        }
        std::cout << std::endl;
    }

}

// ===================================================================
// GameSystemで行うMap更新処理
// UnitData,ShadowData,地形MapDataを元にMapDataを更新する
// ===================================================================
void MapSystemComponent::UpdateMap(const std::vector<UnitStatus*>& units,
    const int* const* shadowMap)
{
    if (!m_pOwner) return;

    //=======================================
    // マップを初期化（地形のみ残す）
    //=======================================
    for (int z = 0; z < m_MapHeight; ++z)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            // 地形はそのまま、それ以外はEmptyに
            if (m_MapData[z][x] != (int)EMapTile::Wall &&
                m_MapData[z][x] != (int)EMapTile::Tree)
            {
                m_MapData[z][x] = (int)EMapTile::Empty;
            }
        }
    }

    //=======================================
    // UnitSystemからプレイヤー・敵の位置を取得・反映
    //=======================================
    /*
        想定するUnitSystemの関数・データ：
        - static const std::vector<Unit*>& GetUnits();
            → 登録されている全UnitのStatusを返す
        - Unit側で持っている情報：
            マップ上のX座標 マップ上のZ座標 プレイヤーかどうか
    */
    
    for(auto* unit : units)
    {
        if (unit->isDown) continue;
        
        // 位置情報取得・反映
        switch (unit->type)
        {
        case UnitType::Player:
            m_MapData[unit->pos.z][unit->pos.x] = (int)EMapTile::Player;
            break;
        case UnitType::Enemy:
            m_MapData[unit->pos.z][unit->pos.x] = (int)EMapTile::Enemy;
            break;
        }
    }

    //=======================================
    // ShadowSystemから影情報を取得・反映
    // 空いているセルのみ反映
    //=======================================
    /*
        想定するShadowSystemの関数：
        - static bool IsShadowAt(int x, int z);
            → 座標(x,z)に影があるかどうか返す
        - static void AddShadow(int x, int z);    // 影を登録
        - static void ClearShadows();              // 毎フレームリセット
    */
    for (int z = 0; z < m_MapHeight; ++z)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            if (m_MapData[z][x] == (int)EMapTile::Empty &&
                shadowMap[z][x] == 1)
            {
                m_MapData[z][x] = (int)EMapTile::Shadow;
            }
        }
    }
    
}
// ===================================================================
// 生マップデータ取得
// ===================================================================
const int* const* MapSystemComponent::GetRawMapData() const
{
    return m_MapData;
}
