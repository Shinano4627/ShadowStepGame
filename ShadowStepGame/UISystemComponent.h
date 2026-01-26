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

class UISystemComponent : public Component
{
private:

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
        
    }

    // ===================================================================
    // 内部処理
    // ===================================================================
    void SetUIObject(std::vector<std::unique_ptr<GameObject>>& objectList);     // UIの初期設定
private:
    // ボタン整列処理
    //void MakeUIButtons(std::vector<std::unique_ptr<GameObject>>& objectList);
    //void MakeUITimeLine(std::vector<std::unique_ptr<GameObject>>& objectList);
    //void MakeUIStatus(std::vector<std::unique_ptr<GameObject>>& objectList);
    //void MakeUISunRoute(std::vector<std::unique_ptr<GameObject>>& objectList);
};