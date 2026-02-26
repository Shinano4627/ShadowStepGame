// ===================================================================
// SceneProto.cpp
// Plese Write scene explanation
// ===================================================================
#include "SceneProto.h"
#include "SceneManager.h"
#include "IOManager.h"
#include "CursorManager.h"
#include "SoundManager.h"
#include <iostream>

// Components
#include "SimplePlaneRendererComponent.h"
#include "MapSystemComponent.h"
#include "UnitComponent.h"
#include "OrbitCameraComponent.h"
#include "GameSystemComponent.h"
#include "SunManageComponent.h"
#include "ShadowSystemComponent.h"
#include "UnitSystemComponent.h"
#include "UISystemComponent.h"
#include "MeshRendererComponent.h"

#include <fstream>
#include <sstream>
#include <stdio.h>
using namespace std;

using namespace DirectX::SimpleMath;


void SceneProto::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneProto] Init START" << std::endl;

    if (m_GameObjectList == nullptr)
    {
        // リストクラスのインスタンス作成
        m_GameObjectList = std::make_unique<GameObjectList>();
    }
    // Delete ObjectList
    m_GameObjectList->DeleteObjectList();

    // Make ObjectList
    m_GameObjectList->MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_PROTO).c_str());

    // 追加コンポーネント
    {
        // マップシステム
        auto* mapSystem = m_GameObjectList->FindGameObjectWithTag("System")->AddComponent<MapSystemComponent>("TestMap.csv");
        mapSystem->MakeMap(m_GameObjectList);    // マップの読み込み
        mapSystem->MakeSelectMap(m_GameObjectList); // セレクトマップの作製
        int heightMap = mapSystem->GetMapSizeHeight();
        int widthMap = mapSystem->GetMapSizeWidth();

        // 太陽
        auto* sun = m_GameObjectList->FindGameObjectWithTag("System")->AddComponent<SunManageComponent>(
            widthMap, heightMap, m_MaxSunMoveTurn);

        // シャドウシステム
        auto* shadowSystem = m_GameObjectList->FindGameObjectWithTag("System")->AddComponent<ShadowSystemComponent>();
        
        // ユニットシステム
        auto* unitSystem = m_GameObjectList->FindGameObjectWithTag("System")->AddComponent<UnitSystemComponent>();

        // UIシステム
        auto* uiSystem = m_GameObjectList->FindGameObjectWithTag("System")->AddComponent<UISystemComponent>();
        uiSystem->SetUIObject(m_GameObjectList);     // UI作成

        // ゲームシステム
        auto* gameSystem = m_GameObjectList->FindGameObjectWithTag("System")->AddComponent<GameSystemComponent>();
        gameSystem->InitGame(m_GameObjectList); // ゲームシステム開始

        // カメラ
        auto* orbitCamera = m_GameObjectList->FindGameObjectWithTag("System")->AddComponent<OrbitCameraComponent>(&m_Camera);
        orbitCamera->SetGameSystem(gameSystem);
        orbitCamera->SetRotationSpeed(0.02f);


    // ここで SunManageComponent をセット
    // ※m_sunSystem は現状 private なので、public setter または friend でアクセス推奨
    // gameSystem->SetSunSystem(sun); // setter を作ると良い

    }  

    MakeUnit();

    // Init Camera
    m_Camera.Init();
    m_UiCamera.Init();

    // Init Data
    m_nextScene = SCENE_NONE;

    // BGMの開始
    SOUND_MANAGER.PlayBGM(SOUND_LABEL::SOUND_LABEL_BGM_GAME);

    // Complete
    m_isInitialized = true;

    std::cout << "[SceneProto] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
}

void SceneProto::UnInit()
{
    std::cout << "[SceneProto] UnInit" << std::endl;
    m_GameObjectList->DeleteObjectList();

    // UnInit Camera
    m_Camera.Uninit();
    m_UiCamera.Uninit();

    // BGMの停止
    SOUND_MANAGER.Stop(SOUND_LABEL::SOUND_LABEL_BGM_GAME);

    // Complete
    m_isInitialized = false;
}

void SceneProto::Update()
{

    // 1. カメラ更新
    m_Camera.Update();
    m_UiCamera.Update();

    // ゲーム用カーソルアップデート
    CURSOR_MANAGER.Update();

    // システムアップデート
    auto* gameSystem = m_GameObjectList->FindGameObjectWithTag("System")->GetComponent<GameSystemComponent>();
    gameSystem->UpdateGame(m_GameObjectList);   // リスト権限を渡して各種アップデート
    if (gameSystem->GameEnd_flg == true)
    {
        std::cout << "[SceneProto] Ended - Next Result" << std::endl;
        m_nextScene = SCENE_RESULT;
    }

    // 2. 全GameObject更新
    m_GameObjectList->UpdateObjectList();

}



void SceneProto::Draw()
{
    // World
    Draw(&m_Camera);

    // Ui
    m_GameObjectList->DrawLayer(&m_UiCamera, RenderLayer::UI);
    m_GameObjectList->DrawLayer(&m_UiCamera, RenderLayer::UI_2);

    // カーソルを最前面に描画
    CURSOR_MANAGER.Draw();
}

void SceneProto::Draw(Camera* camera)
{
    m_GameObjectList->DrawLayer(camera, RenderLayer::WORLD);
}


// Unitを.csvファイルから作成
void SceneProto::MakeUnit()
{
    m_DataFile = "data/testunit.csv";

    // .csvData変数
    ifstream csv_data(m_DataFile, ios::in);

    // 各Component
    // ユニットシステム
    auto* unitSystem = m_GameObjectList->FindGameObjectWithTag("System")->GetComponent<UnitSystemComponent>();
    auto* mapSystem = m_GameObjectList->FindGameObjectWithTag("System")->GetComponent<MapSystemComponent>();


    // 既存データ削除


    // CSVを開く
    if (!csv_data.is_open()) 
    {
        cout << "Error: opening file fail" << endl;
        exit(1);
    }
    
    cout << "Start Read : " << m_DataFile << endl;

    string line;
    istringstream sin;
    string word;

    m_MapWidth = 0;     // CSVから読み取り
    m_MapHeight = 0;    // CSVから読み取り

    // ------------データ読み取り-----------------

    // マップの広さを取得
    getline(csv_data, line);
    sin.clear();
    sin.str(line);

    //文字列ストリームsinの文字をコンマ区切り
    getline(sin, word, ',');
    m_MapWidth = stoi(word);
    getline(sin, word, ',');
    m_MapHeight = stoi(word);

    int z = 0;

    int pl_x, pl_z;

    int u_ID = 2000;
    int p_Speed = 30;
    int e_Speed = 10;
    
    // 行ごとにデータを読み込む
    while (getline(csv_data, line)) {
        sin.clear();
        sin.str(line);
        int x = 0;

        // CSV は上から下に並んでいるので反転させる
        int realZ = m_MapHeight - 1 - z;

        while (getline(sin, word, ',')) {
            int data = std::stoi(word);

            if (data == 0) { x++; continue; }

            // Map座標 → PlayerMapPos変換
            mapSystem->ConvertMapIndexToUnitPos(x, realZ, pl_x, pl_z);

            UnitStatus unit_S;
            unit_S.id = u_ID;
            unit_S.hp = 10;
            unit_S.isDown = false;
            unit_S.pos = MapPosition(pl_x, pl_z);

            // Obj生成
            auto obj = std::make_unique<GameObject>(
                Vector3(pl_x * 5, 0.f, pl_z * 5),
                Vector3::Zero,
                Vector3(2.5f, 2.5f, 2.5f)
            );
            GameObject* newObject = obj.get();
            newObject->SetID(u_ID);
            Transform& transform = newObject->GetTransform();

            if (data == 1 ||
                data == 2 ||
                data == 3)
            {
                // Player
                unit_S.type = UnitType::Player;
                unit_S.speed = p_Speed;
                newObject->SetName("Player");
                newObject->SetTag("Player");
                switch (data) {
                case 1:
                    // Attack
                    unit_S.model = UnitModel::Attack;
                    // Mesh
                    {
                        // テンプレートからモデル情報を取得
                        auto playerTemplate = m_GameObjectList->FindGameObjectWithName("PlayerAttackTemplate");
                        if (!playerTemplate) return;
                        auto* templateMesh = playerTemplate->GetMeshComponent<MeshRendererComponent>();
                        if (!templateMesh) return;
                        Vector3 newScale = transform.GetScale() * playerTemplate->GetTransform().GetScale();
                        transform.SetScale(newScale);

                        // メッシュコンポーネントを追加
                        auto mesh = newObject->AddMeshComponent<MeshRendererComponent>(
                            templateMesh->GetModelPath(),
                            templateMesh->GetTexturePath()
                        );
                        mesh->LoadModel();
                        // アニメーション情報取得
                        templateMesh->CopyAnimations(mesh);
                    }
                    break;
                case 2:
                {
                    // Place
                    unit_S.model = UnitModel::Place;
                    // Mesh
                    // テンプレートからモデル情報を取得 TODO:配置と攻撃のモデルをわける
                    auto playerTemplate = m_GameObjectList->FindGameObjectWithName("PlayerAttackTemplate");
                    if (!playerTemplate) return;
                    auto* templateMesh = playerTemplate->GetMeshComponent<MeshRendererComponent>();
                    if (!templateMesh) return;
                    Vector3 newScale = transform.GetScale() * playerTemplate->GetTransform().GetScale();
                    transform.SetScale(newScale);

                    // メッシュコンポーネントを追加
                    auto mesh = newObject->AddMeshComponent<MeshRendererComponent>(
                        templateMesh->GetModelPath(),
                        templateMesh->GetTexturePath()
                    );
                    mesh->LoadModel();
                    // アニメーション情報取得
                    templateMesh->CopyAnimations(mesh);
                }
                    break;
                case 3:
                    // Giant
                    unit_S.model = UnitModel::Giant;
                    // Mesh
                    newObject->AddMeshComponent<SimpleCubeRendererComponent>
                        (Color(1.0f, 0.0f, 0.0f, 1.0f));
                    break;
                }

                p_Speed++;
            }
            else if (data == 4 ||
                data == 5 ||
                data == 6)
            {
                // Enemy
                unit_S.type = UnitType::Enemy;
                unit_S.speed = e_Speed;
                newObject->SetName("Enemy");
                newObject->SetTag("Enemy");
                switch (data) {
                case 4:
                    // Attack
                    unit_S.model = UnitModel::Attack;
                    // Mesh
                    {
                        // テンプレートからモデル情報を取得
                        auto enemyTemplate = m_GameObjectList->FindGameObjectWithName("EnemyTemplate");
                        if (!enemyTemplate) return;
                        auto* templateMesh = enemyTemplate->GetMeshComponent<MeshRendererComponent>();
                        if (!templateMesh) return;
                        Vector3 newScale = transform.GetScale() * enemyTemplate->GetTransform().GetScale();
                        transform.SetScale(newScale);

                        // メッシュコンポーネントを追加
                        auto mesh = newObject->AddMeshComponent<MeshRendererComponent>(
                            templateMesh->GetModelPath(),
                            templateMesh->GetTexturePath()
                        );

                        mesh->LoadModel();
                        // アニメーション情報取得
                        templateMesh->CopyAnimations(mesh);
                    }
                    break;
                case 5:
                    // Place
                    unit_S.model = UnitModel::Place;
                    // Mesh
                    {
                        // テンプレートからモデル情報を取得
                        auto enemyTemplate = m_GameObjectList->FindGameObjectWithName("EnemyTemplate");
                        if (!enemyTemplate) return;
                        auto* templateMesh = enemyTemplate->GetMeshComponent<MeshRendererComponent>();
                        if (!templateMesh) return;
                        Vector3 newScale = transform.GetScale() * enemyTemplate->GetTransform().GetScale();
                        transform.SetScale(newScale);

                        // メッシュコンポーネントを追加
                        auto mesh = newObject->AddMeshComponent<MeshRendererComponent>(
                            templateMesh->GetModelPath(),
                            templateMesh->GetTexturePath()
                        );
                        mesh->LoadModel();

                        // アニメーション情報取得
                        templateMesh->CopyAnimations(mesh);
                    }
                    break;
                case 6:
                    // Giant
                    unit_S.model = UnitModel::Giant;
                    // Mesh
                    newObject->AddMeshComponent<SimpleCubeRendererComponent>
                        (Color(0.0f, 0.0f, 1.0f, 1.0f));
                    break;
                }

                e_Speed++;
            }

            cout << "Create:Unit" << endl;
            // UnitComponentをAdd
            auto* Unit = newObject->AddComponent<UnitComponent>();
            Unit->Init();
            Unit->SetStatus(unit_S);

            m_GameObjectList->AddObject(std::move(obj));
            
            unitSystem->RegisterUnit(Unit);
            x++;
            u_ID++;
        }
        z++;
    }

    csv_data.close();
}