// ===================================================================
// SceneGame.h
// ゲームシーン - プレイヤー移動と床の表示
// ===================================================================
#pragma once
#include "SceneBase.h"

class SceneGame : public SceneBase
{
private:
    // オブジェクト管理用ID
    enum OBJECT_ID
    {
        ID_PLAYER,
        ID_FLOOR,

        ID_COUNT,
    };

    float m_GameTime = 0.f; // ゲーム経過時間

public:
    void Init() override;
    void UnInit() override;
    void Update() override;
    void Draw() override;
    void Draw(Camera*) override;

private:
    // ボタン整列処理
    void MakeUIButtons();
    void MakeUITimeLine();
    void MakeUIStatus();
    void MakeUISunRoute();
};
