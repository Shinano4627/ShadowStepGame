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

// 前方宣言
class GameObjectList;

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
    void SetUIObject(std::unique_ptr<GameObjectList>& objectList);     // UIの初期設定
private:
    // ボタン整列処理
    void MakeUIButtons(std::unique_ptr<GameObjectList>& objectList);
    void MakeUITimeLine(std::unique_ptr<GameObjectList>& objectList);
    void MakeUIStatus(std::unique_ptr<GameObjectList>& objectList);
    void MakeUISunRoute(std::unique_ptr<GameObjectList>& objectList);
};