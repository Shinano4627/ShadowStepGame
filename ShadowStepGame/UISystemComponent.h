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
#include "UnitCommon.h"
#include <map>

// 前方宣言
class GameObjectList;

class UISystemComponent : public Component
{
private:
    UnitActionType m_CurrentSellected = UnitActionType::None;
    GameObjectList* m_pUIObjects = nullptr; //UIオブジェクト一覧
    std::map<UnitActionType, ButtonComponent*> m_ActionButtonMap; // UnitActionTypeとボタンの紐付け
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

        // キーで検索
        for (auto buttonPair : m_ActionButtonMap)
        {
            if (buttonPair.second->IsSelected())
            {
                UnitActionType currentSellected = buttonPair.first; // キーを設定
                if (currentSellected != m_CurrentSellected)
                {
                    std::cout << "[UI] Hovered Button: "
                        << UIButtonTypeToString(GetSelectedButton())
                        << std::endl;
                }
                m_CurrentSellected = currentSellected;
                return; // 一つのみを検知
            }
        }

        // どのボタンも選択されていない
        m_CurrentSellected = UnitActionType::None;
        
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
    }

    UnitActionType GetSelectedButton()const { return m_CurrentSellected; };
    // UIButtonType を文字列に変換するヘルパー関数
    const char* UIButtonTypeToString(UnitActionType type)
    {
        switch (type)
        {
        case UnitActionType::None:   return "None";
        case UnitActionType::Place:  return "Build";        // TODO：設置の名称を統一したい
        case UnitActionType::Move:   return "Move";
        case UnitActionType::Attack: return "Attack";
        case UnitActionType::ShadowMove: return "ShadowMove";
        default: return "Unknown";
        }
    }

    // ===================================================================
    // 内部処理
    // ===================================================================
    void SetUIObject(std::unique_ptr<GameObjectList>& objectList);     // UIの初期設定
    void UpdateDisplayButton(UnitModel model);

    // ===================================================================
    // UnitActionTypeとボタンの紐付け管理
    // ===================================================================
    void RegisterActionButton(UnitActionType type, ButtonComponent* button);  // 紐付けを登録
    ButtonComponent* GetButtonByAction(UnitActionType type) const;            // UnitActionTypeからボタンを取得
private:
    bool test = false;

    // ボタン整列処理
    void MakeUIButtons(std::unique_ptr<GameObjectList>& objectList);
    void MakeUITimeLine(std::unique_ptr<GameObjectList>& objectList);
    void MakeUIStatus(std::unique_ptr<GameObjectList>& objectList);
    void MakeUISunRoute(std::unique_ptr<GameObjectList>& objectList);

};