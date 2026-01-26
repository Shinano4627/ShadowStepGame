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
#include "CursorManager.h"

using namespace std;


// コンポーネント
#include "PlayerMoverComponent.h"
#include "Texture2D.h"

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
        // ボタン整列処理
        MakeButtons();

        std::cout << "[SceneGame] Player created" << std::endl;
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

    // ゲーム用カーソルアップデート
    CURSOR_MANAGER.Update();

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
    DrawLayer(nullptr, RenderLayer::UI);

    // カーソルを最前面に描画
    CURSOR_MANAGER.Draw();
}

void SceneGame::Draw(Camera* camera)
{
    // WORLD層を描画（カメラ使用）
    DrawLayer(camera, RenderLayer::WORLD);
}

// ===================================================================
// ボタン整列処理
// ===================================================================
void SceneGame::MakeButtons()
{
    // UIAreaButtonsの範囲を取得
    GameObject* buttonArea = FindGameObjectWithTag("UIArea");
    for (auto& obj : m_GameObjects)
    {
        if (obj && obj->GetName() == "UIAreaButtons")
        {
            buttonArea = obj.get();
            break;
        }
    }

    if (!buttonArea)
    {
        std::cout << "[SceneGame] UIAreaButtons not found!" << std::endl;
        return;
    }

    Vector3 areaPos = buttonArea->GetTransform().GetPosition();
    Vector3 areaScale = buttonArea->GetTransform().GetScale();

    // パディング（仮値）
    const float padding = 5.f;

    // 行数: ButtonSub(1行) + Button(2行) = 3行
    const int rowCount = 3;
    float rowHeight = (areaScale.y - padding * (rowCount + 1)) / rowCount;

    // 各行のY座標を計算（上から順に）
    // エリアの上端からスタート
    float topY = areaPos.y + areaScale.y / 2.f;

    // ---------------------------------------------------------------
    // 1行目: ButtonSub（横並び）
    // ---------------------------------------------------------------
    std::vector<GameObject*> buttonSubs = FindGameObjectsWithTag("ButtonSub");
    int subCount = static_cast<int>(buttonSubs.size());

    if (subCount > 0)
    {
        float row1Y = topY - padding - rowHeight / 2.f;
        float subWidth = (areaScale.x - padding * (subCount + 1)) / subCount;
        float startX = areaPos.x - areaScale.x / 2.f + padding + subWidth / 2.f;

        for (int i = 0; i < subCount; i++)
        {
            float posX = startX + i * (subWidth + padding);
            buttonSubs[i]->GetTransform().SetPosition(Vector3(posX, row1Y, 0.f));
            buttonSubs[i]->GetTransform().SetScale(Vector3(subWidth, rowHeight, 1.f));
            // UVを設定
            auto* tex = buttonSubs[i]->GetMeshComponent<Texture2D>();
            tex->SetUV(1, 1, 2, 1);
        }
    }

    // ---------------------------------------------------------------
    // 2行目・3行目: Button（ButtonBuild, ButtonMove）
    // ---------------------------------------------------------------
    // 表示するボタン名のリスト
    std::vector<std::string> buttonNames = { "ButtonBuild", "ButtonMove" };

    // TagがButtonのオブジェクトから該当するものを取得
    std::vector<GameObject*> buttons = FindGameObjectsWithTag("Button");
    std::vector<GameObject*> targetButtons;

    for (const auto& name : buttonNames)
    {
        for (auto* btn : buttons)
        {
            if (btn->GetName() == name)
            {
                targetButtons.push_back(btn);
                break;
            }
        }
    }

    // 不要なButtonは非アクティブに
    for (auto* btn : buttons)
    {
        bool isTarget = false;
        for (auto* target : targetButtons)
        {
            if (btn == target)
            {
                isTarget = true;
                break;
            }
        }
        if (!isTarget)
        {
            btn->SetActive(false);
        }
    }

    // ボタンを配置（2行目、3行目）
    float buttonWidth = areaScale.x - padding * 2;
    for (int i = 0; i < static_cast<int>(targetButtons.size()); i++)
    {
        // 行インデックス（1行目はButtonSubなので、i+1）
        int rowIndex = i + 1;
        float rowY = topY - padding - rowHeight / 2.f - rowIndex * (rowHeight + padding);
        float posX = areaPos.x;

        targetButtons[i]->GetTransform().SetPosition(Vector3(posX, rowY, 0.f));
        targetButtons[i]->GetTransform().SetScale(Vector3(buttonWidth, rowHeight, 1.f));

        // UVを設定
        auto* tex = targetButtons[i]->GetMeshComponent<Texture2D>();
        tex->SetUV(1, 1, 2, 1);
    }

    std::cout << "[SceneGame] Buttons arranged in UIAreaButtons" << std::endl;
}
