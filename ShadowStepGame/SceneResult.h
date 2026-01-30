// ===================================================================
// SceneResult.h
// リザルトシーン - OBJモデル読み込みとカメラ追従
// ===================================================================
#pragma once
#include "SceneBase.h"
#include "Camera.h"

class SceneResult : public SceneBase
{
private:
    // オブジェクト管理用ID
    enum OBJECT_ID
    {
        ID_MODEL_OBJECT,    // OBJモデル表示用

        ID_COUNT,
    };

    float m_DisplayTime = 0.f;    // 表示時間
    
public:
    void Init() override;
    void UnInit() override;
    void Update() override;
    void Draw() override;
    void Draw(Camera*) override;
};