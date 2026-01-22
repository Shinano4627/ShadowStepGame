// ===================================================================
// SceneTitle.h
// タイトルシーン - GameObjectとコンポーネントシステムを使用
// ===================================================================
#pragma once
#include "SceneBase.h"

class SceneTitle : public SceneBase
{
private:
    // オブジェクト管理用ID
    enum OBJECT_ID
    {
        ID_TITLE_CUBE,

        ID_COUNT,
    };

public:
    void Init() override;
    void UnInit() override;
    void Update() override;
    void Draw() override;
    void Draw(Camera*) override;
};