// ===================================================================
// SceneGame.cpp
// ゲームシーン実装
// ===================================================================
#include "SceneGame.h"
#include "SceneManager.h"
#include "IOManager.h"
#include "Game.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "SimplePlaneRendererComponent.h"

using namespace std;


// コンポーネント
#include "PlayerMoverComponent.h"

#include <iostream>

void SceneGame::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneGame] Init START" << std::endl;

    // 既存オブジェクトを削除
    DeleteObjectList();

    // ゲーム時間初期化
    m_GameTime = 0.0f;

    using namespace DirectX::SimpleMath;

    // オブジェクトリスト作成
    MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_GAME).c_str());

    // 追加コンポーネント
    {
        // プレイヤー移動コンポーネント
        auto* mover = FindGameObjectWithTag("Player")->AddComponent<PlayerMoverComponent>(5.0f, 3.0f);

        std::cout << "[SceneGame] Player created" << std::endl;
    }
    //マップレーダー
    
    int mapZ = 0;
    int mapX = 0;
    int n = 3000;
    ifstream csv_data("data/testmap.csv", ios::in);

    if (!csv_data.is_open()) {
        cout << "Error: opening file fail" << endl;
        exit(1);
    }
    else {
        string line;

        vector<string> words; //文字列ベクトルを宣言する
        string word;
        // ------------データ読み取り-----------------
        // 行ごとにデータを読み込む
        //getline(csv_data, line);


        istringstream sin;
        // 行ごとにデータを読み込む
        while (getline(csv_data, line)) {
            // vectorおよび文字ストリームをクリアし、前の行のデータのみを保存します
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
                auto obj = std::make_unique<GameObject>(Vector3(mapX * 5.f, 0.2f, mapZ * 5), Vector3::Zero, Vector3(3.f, 1.f, 3.f));
                GameObject* newObject = obj.get();
                newObject->SetID(n);
                newObject->SetName("Map");
                newObject->SetTag("Map");
                cout << "check2";

                // CSVからのよみとり
                Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
                switch (stoi(str))
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
                default:
                    break;
                }
                newObject->AddMeshComponent<SimplePlaneRendererComponent>(color);
                mapX++;
                n++;
                m_GameObjects.push_back(std::move(obj));
            }
            mapX = 0;
            mapZ++;
            //return words;
        }


        csv_data.close();
    }



    // カメラ初期化
    m_Camera.Init();

    m_nextScene = SCENE_NONE;

    // 初期化完了
    m_isInitialized = true;

    std::cout << "[SceneGame] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "=== GAME SCENE ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  W/A/S/D - Move Player" << std::endl;
    std::cout << "  Q/E     - Rotate Player" << std::endl;
    std::cout << "  ENTER   - Go to Result" << std::endl;
    std::cout << "  ESC     - Back to Title" << std::endl;
    std::cout << "" << std::endl;
}

void SceneGame::UnInit()
{
    std::cout << "[SceneGame] UnInit" << std::endl;
    DeleteObjectList();

    // カメラ終了処理
    m_Camera.Uninit();

    m_isInitialized = false;
}

void SceneGame::Update()
{
    // ゲーム時間更新
    m_GameTime += Game::GetDeltaTime();

    // カメラ更新
    m_Camera.Update();

    // Enterキーでリザルトへ
    if (IO_MANAGER.GetKeyDown(TYPE_OK) || IO_MANAGER.GetKeyDownKeyBord(VK_RETURN))
    {
        std::cout << "[SceneGame] ENTER pressed - Go to Result" << std::endl;
        m_nextScene = SCENE_RESULT;
        return;
    }

    // Escキーでタイトルへ戻る
    if (IO_MANAGER.GetKeyDownKeyBord(VK_ESCAPE))
    {
        std::cout << "[SceneGame] ESC pressed - Back to Title" << std::endl;
        m_nextScene = SCENE_TITLE;
        return;
    }

    // GameObjectリストを更新（プレイヤー移動など）
    UpdateObjectList();
}

void SceneGame::Draw()
{
    // 3D描画
    Draw(&m_Camera);

    // UI層のみ描画（カメラ不使用）
    DrawLayer(&m_Camera, RenderLayer::UI);
}

void SceneGame::Draw(Camera* camera)
{
    // WORLD層を描画（カメラ使用）
    DrawLayer(camera, RenderLayer::WORLD);
}