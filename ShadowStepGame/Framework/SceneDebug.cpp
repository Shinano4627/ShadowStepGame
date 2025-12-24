// ===================================================================
// SceneDebug.cpp
// デバッグ用シーン実装
// ===================================================================
#include "SceneDebug.h"

#include <iostream>

#include "SceneManager.h"
#include "IOManager.h"
#include "DebugUI.h"

void SceneDebug::Init()
{
    std::cout << "========================================" << std::endl;
    std::cout << "[SceneDebug] Init START" << std::endl;

    // 既存オブジェクトを削除
    DeleteObjectList();

    using namespace DirectX::SimpleMath;

    // オブジェクトリスト作成
    MakeObjectList(SCENE_MANAGER.GetSceneName(SCENE_TITLE).c_str());

    // カメラ初期化
    m_Camera.Init();

    // とりあえずIDが一番小さいオブジェクトをアクティブにする
    m_activeObjectDebug = m_GameObjects[0].get();
    DebugUIInit();

    m_nextScene = SCENE_NONE;

    // 初期化完了
    m_isInitialized = true;

    std::cout << "[SceneDebug] Initialized successfully" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "=== TITLE SCENE ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  ENTER - Start Game" << std::endl;
    std::cout << "  SPACE - Go to TestCube" << std::endl;
    std::cout << "" << std::endl;
}

void SceneDebug::UnInit()
{
    std::cout << "[SceneDebug] UnInit" << std::endl;
    DeleteObjectList();

    DebugUIUnInit();

    // カメラ終了処理
    m_Camera.Uninit();

    m_isInitialized = false;
}

void SceneDebug::Update()
{
    // カメラ更新
    m_Camera.Update();

    // Backキーでタイトルに戻る
    if (IO_MANAGER.GetKeyDownKeyBord(VK_BACK))
    {
        ExitDebug();
        return;
    }

    // Ctrl＋Sキーでxmlファイルを保存
    if (IO_MANAGER.GetKeyPressKeyBord(VK_CONTROL)&& IO_MANAGER.GetKeyDownKeyBord(VK_S))
    {
        SaveData();
    }

    if (m_curScene != m_preScene)
    {
        ReLoadObject();
    }

    // GameObjectリストを更新
    UpdateObjectList();
}

void SceneDebug::Draw()
{
    // UI層のみ描画（カメラ不使用）
    DrawLayer(&m_Camera, RenderLayer::UI);

    // 3D描画
    Draw(&m_Camera);
}

void SceneDebug::Draw(Camera* camera)
{
    // WORLD層を描画（カメラ使用）
    DrawLayer(camera, RenderLayer::WORLD);

    DebugUIRender();
}

/// <summary>
/// オブジェクトの再読み込み
/// </summary>
void SceneDebug::ReLoadObject()
{
    // 既存オブジェクトを削除
    DeleteObjectList();

    // オブジェクトリスト作成
    MakeObjectList(SCENE_MANAGER.GetSceneName((SCENE)m_curScene).c_str());

    // とりあえずIDが一番小さいオブジェクトをアクティブにする
    m_activeObjectDebug = m_GameObjects[0].get();

    // 作成完了をもって更新する
    m_preScene = m_curScene;    
}

// ===================================================================
// Transformの調整
// ===================================================================
void SceneDebug::DebugUI()
{
	ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Once);
	ImGui::Begin("Transform", nullptr, 0);

	{
        // シーンのコンボボックスの作成
        if (ImGui::BeginCombo("Scene", SCENE_MANAGER.GetSceneNameAll()[m_curScene].c_str()))
        {
            for (int i = 0; i < SCENE_MANAGER.GetSceneNameAll().size(); i++)
            {
                bool isSelected = (m_curScene == i);
                if (ImGui::Selectable(SCENE_MANAGER.GetSceneNameAll()[i].c_str(), isSelected))
                {
                    m_curScene = i; // 選択変更
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        std::vector<std::string> objectNameList;
        for (auto& gameObject : m_GameObjects)
        {
            objectNameList.push_back(gameObject->GetName());
        }
        int curID = m_activeObjectDebug->GetID();

        // オブジェクトのコンボボックスの作成
        if (ImGui::BeginCombo("Object", objectNameList[curID].c_str()))
        {
            for (int i = 0; i < objectNameList.size(); i++)
            {
                bool isSelected = (curID == i);
                if (ImGui::Selectable(objectNameList[i].c_str(), isSelected))
                {
                    curID = i; // 選択変更
                    m_activeObjectDebug = m_GameObjects[curID].get();
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        // トランスフォームデータ
        {
            float position[3] =
            {
                m_activeObjectDebug->GetTransform().GetPosition().x,
                m_activeObjectDebug->GetTransform().GetPosition().y,
                m_activeObjectDebug->GetTransform().GetPosition().z,
            };
            float rotation[3] =
            {
                m_activeObjectDebug->GetTransform().GetRotation().x,
                m_activeObjectDebug->GetTransform().GetRotation().y,
                m_activeObjectDebug->GetTransform().GetRotation().z,
            };
            float scale[3] =
            {
                m_activeObjectDebug->GetTransform().GetScale().x,
                m_activeObjectDebug->GetTransform().GetScale().y,
                m_activeObjectDebug->GetTransform().GetScale().z,
            };

            ImGui::SliderFloat3("Position", position, -100.0f, 100.0f);
            ImGui::SliderFloat3("Rotation", rotation, 0.0f, 100.0f);
            ImGui::SliderFloat3("Scale", scale, 0.0f, 100.0f);

            // 変更した値をセットする
            m_activeObjectDebug->GetTransform().SetPosition(Vector3(position[0], position[1], position[2]));
            m_activeObjectDebug->GetTransform().SetRotation(Vector3(rotation[0], rotation[1], rotation[2]));
            m_activeObjectDebug->GetTransform().SetScale(Vector3(scale[0], scale[1], scale[2]));
        }

        // ボタン
        if (ImGui::Button("Save")) {
            SaveData();
        }
        if (ImGui::Button("Exit")) {
            ExitDebug();
        }
	}

	// カメラの位置を極座標からデカルト座標に変換
	ImGui::End();
}

void SceneDebug::DebugUIInit()
{
	DebugUI::RedistDebugFunction(std::bind(&SceneDebug::DebugUI, this));
}

void SceneDebug::DebugUIUnInit()
{
	DebugUI::DisposeUI();
}

void SceneDebug::DebugUIRender()
{
	// DebugUIの描画
	DebugUI::Render();
}

void SceneDebug::SaveData()
{
    int res = MessageBoxA(NULL, "保存しますか？", "確認", MB_OKCANCEL);
    if (res == IDOK) {
        std::cout << "[SceneDebug] Save pressed..." << std::endl;
        SaveObjectData(SCENE_MANAGER.GetSceneNameAll()[m_curScene].c_str());
    }
}

void SceneDebug::ExitDebug()
{
    int res = MessageBoxA(NULL, "デバッグモードを終了しますか？", "確認", MB_OKCANCEL);
    if (res == IDOK) {
        m_nextScene = SCENE_TITLE;
        SaveData();
    }
}