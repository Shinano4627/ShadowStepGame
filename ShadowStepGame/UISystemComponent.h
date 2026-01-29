#pragma once
// ===================================================================
// UISystemComponent.h
// ゲーム画面のUI制御
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include "ButtonComponent.h"
#include "GameObjectList.h"
#include "UnitComponent.h"

// 前方宣言
class GameObjectList;

class UISystemComponent : public Component
{
private:
    ButtonComponent::UIButtonType m_CurrentHover = ButtonComponent::UIButtonType::None;
    GameObjectList* m_pUIObjects = nullptr; //UIオブジェクト一覧
public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    UISystemComponent()
    {       
    }

    // ===================================================================
    // 初期化処理
    // ===================================================================
    void Init() override
    {
    }
    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {

        if (!m_pOwner) return;


        // Button タグのオブジェクト取得
        std::vector<GameObject*> buttons =
            m_pUIObjects->FindGameObjectsWithTag("Button");

        ButtonComponent::UIButtonType hover =
            ButtonComponent::UIButtonType::None;

        // どれか1つでもカーソルに触れていたらそれを採用
        for (auto* obj : buttons)
        {
            auto* btn = obj->GetComponent<ButtonComponent>();
            if (!btn) continue;

            if (btn->IsSelected())
            {
                hover = btn->GetButtonType();
                break;
            }
        }

        //// 状態が変わったときだけログ
        //if (hover != m_CurrentHover)
        //{
        //    m_CurrentHover = hover;

        //    switch (m_CurrentHover)
        //    {
        //    case ButtonComponent::UIButtonType::Build:
        //        std::cout << "[UISystem] Hover: Build Button" << std::endl;
        //        break;

        //    case ButtonComponent::UIButtonType::Move:
        //        std::cout << "[UISystem] Hover: Move Button" << std::endl;
        //        break;

        //    default:
        //        std::cout << "[UISystem] Hover: None" << std::endl;
        //        break;
        //    }
        //}
        if (hover != m_CurrentHover)
        {
            std::cout << "[UI] Hovered Button: "
                << UIButtonTypeToString(GetSelectedButton())
                << std::endl;
        }
    }

    ButtonComponent::UIButtonType GetSelectedButton()const;
    // UIButtonType を文字列に変換するヘルパー関数
    const char* UIButtonTypeToString(ButtonComponent::UIButtonType type)
    {
        switch (type)
        {
        case ButtonComponent::UIButtonType::None:   return "None";
        case ButtonComponent::UIButtonType::Build:  return "Build";
        case ButtonComponent::UIButtonType::Move:   return "Move";
        case ButtonComponent::UIButtonType::Attack: return "Attack";
        default: return "Unknown";
        }
    }

    // ===================================================================
    // 内部処理
    // ===================================================================
    void SetUIObject(std::unique_ptr<GameObjectList>& objectList);     // UIの初期設定
    void UpdateDisplayButton(UnitComponent::UnitModel model);
private:
    bool test = false;

    // ボタン整列処理
    void MakeUIButtons(std::unique_ptr<GameObjectList>& objectList);
    void MakeUITimeLine(std::unique_ptr<GameObjectList>& objectList);
    void MakeUIStatus(std::unique_ptr<GameObjectList>& objectList);
    void MakeUISunRoute(std::unique_ptr<GameObjectList>& objectList);

};