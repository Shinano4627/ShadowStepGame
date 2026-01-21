// ===================================================================
// SceneSelect.cpp
// Plese Write scene explanation
// ===================================================================
#include "SceneSelect.h"
#include "SceneManager.h"
#include "IOManager.h"
#include <iostream>

// Components
#include "Texture2D.h"

using namespace DirectX::SimpleMath;

void SceneSelect::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneSelect] Init START" << std::endl;

    // Delete ObjectList
    DeleteObjectList();

    // オブジェクトリスト作成
    MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_SELECT).c_str());

    // 追加コンポーネント
    {
        // 不要なボタンを非アクティブに

        // アクティブなボタンを整列して配置
        MakeButton();

        std::cout << "[SceneSelect] TitleCube created" << std::endl;
    }

    // Init Camera
    m_Camera.Init();

    // Init Data
    m_nextScene = SCENE_NONE;

    // Complete
    m_isInitialized = true;

    std::cout << "[SceneSelect] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
}

void SceneSelect::UnInit()
{
    std::cout << "[SceneSelect] UnInit" << std::endl;
    DeleteObjectList();

    // UnInit Camera
    m_Camera.Uninit();

    // Complete
    m_isInitialized = false;
}

void SceneSelect::Update()
{
    // Update Camera
    m_Camera.Update();

    // Enterキーで次のシーン（仮実装）
    if (IO_MANAGER.GetKeyDown(TYPE_OK) || IO_MANAGER.GetKeyDownKeyBord(VK_RETURN))
    {
        std::cout << "[SceneSelect] ENTER pressed - Starting Game" << std::endl;

        // プロトシーン
        if (m_CurrentSelected == 0)
        {
            m_nextScene = SCENE_PROTO;
        }
        // ゲームシーン
        else if (m_CurrentSelected == 1)
        {
            m_nextScene = SCENE_GAME;
        }

        return;
    }

    // 選択状態のボタンの表示を変更
    if (IO_MANAGER.GetKeyDownKeyBord(VK_LEFT))
    {
        // 左のボタンを選択
        UpdateButton(-1);
    }
    if (IO_MANAGER.GetKeyDownKeyBord(VK_RIGHT))
    {
        // 右のボタンを選択
        UpdateButton(1);
    }

    // Update GameObjectList
    UpdateObjectList();
}

void SceneSelect::Draw()
{
    // World
    Draw(&m_Camera);

    // Ui
    DrawLayer(&m_Camera, RenderLayer::UI);

    // メッセージを表示
    DrawLayer(&m_Camera, RenderLayer::UI_Message);
}

void SceneSelect::Draw(Camera* camera)
{
    DrawLayer(camera, RenderLayer::WORLD);
}

void SceneSelect::MakeButton()
{
    std::vector<GameObject*> buttonMesseges = FindGameObjectsWithTag("ButtonMessage");
    // アクティブなButtonMessageのみ抽出
    std::vector<GameObject*> activeMessages;
    for (auto* msg : buttonMesseges)
    {
        if (msg->IsActive()) activeMessages.push_back(msg);
    }

    int count = activeMessages.size();
    // ボタンの最大数を取得
    m_MaxButton = count;

    // ボタンの配置
    const float padding = 10.f;

    // ButtonAreaの範囲を取得
    GameObject* buttonArea = FindGameObjectWithTag("ButtonArea");
    Vector3 areaPos = buttonArea->GetTransform().GetPosition();
    Vector3 areaScale = buttonArea->GetTransform().GetScale();

    // ボタンサイズ計算（縦横比1:1）
    float availableWidth = areaScale.x - padding * (count + 1);
    float buttonWidth = availableWidth / count;
    float buttonHeight = areaScale.y - padding * 2;
    float buttonSize = std::min(buttonWidth, buttonHeight);

    // 元のButtonオブジェクトを取得（テンプレートとして使用）
    GameObject* buttonTemplate = FindGameObjectWithTag("ButtonTemplate");
    std::string texturePath = buttonTemplate->GetMeshComponent<Texture2D>()->GetTexturePath();
    buttonTemplate->SetActive(false); // テンプレートは非表示

    // ボタンを複製して配置
    float startX = areaPos.x - areaScale.x / 2.f + padding + buttonSize / 2.f;
    for (int i = 0; i < count; i++)
    {
        float posX = startX + i * (buttonSize + padding);
        float posY = areaPos.y;

        // 新規Buttonオブジェクト作成
        auto newButton = std::make_unique<GameObject>(
            Vector3(posX, posY, 0.f),
            Vector3(0.f, 0.f, 0.f),
            Vector3(buttonSize, buttonSize, 1.f));
        newButton->SetName("Button_" + std::to_string(i));
        newButton->SetTag("Button");
        auto* tex = newButton->AddMeshComponent<Texture2D>(texturePath);
        tex->SetUV(1, 1, 2, 1);
        m_GameObjects.push_back(std::move(newButton));

        // 対応するButtonMessageを同じ位置に配置
        if (i < static_cast<int>(activeMessages.size()))
        {
            activeMessages[i]->GetMeshComponent<Texture2D>()->SetRenderLayer(RenderLayer::UI_Message);
            activeMessages[i]->GetTransform().SetPosition(Vector3(posX, posY, 0.f));
            activeMessages[i]->GetTransform().SetScale(Vector3(buttonSize, buttonSize, 1.f));
        }
    }

    // 最初のボタンを選択
    m_CurrentSelected = 0;
    // 表示も変更する
    FindGameObjectWithTag("Button")->GetMeshComponent<Texture2D>()->UpdateUV(true, false);
}

void SceneSelect::UpdateButton(int dir)
{
    if (m_MaxButton <= 1) return;

    std::vector<GameObject*> buttons= FindGameObjectsWithTag("Button");

    buttons[m_CurrentSelected]->GetMeshComponent<Texture2D>()->UpdateUV(true, false);
    m_CurrentSelected += dir;

    if (m_CurrentSelected < 0)
    {
        m_CurrentSelected = m_MaxButton - 1;
    }
    else if (m_CurrentSelected >= m_MaxButton)
    {
        m_CurrentSelected = 0;
    }

    buttons[m_CurrentSelected]->GetMeshComponent<Texture2D>()->UpdateUV(true, false);
}