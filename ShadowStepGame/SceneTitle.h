// ===================================================================
// SceneTitle.h
// タイトルシーン - GameObjectとコンポーネントシステムを使用
// ===================================================================
#pragma once
#include "SceneBase.h"

#include "SystemCommon.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "Application.h"
#include "ResourceManager.h"
#include <memory>


class SceneTitle : public SceneBase
{
private:
    // オブジェクト管理用ID
    enum OBJECT_ID
    {
        ID_TITLE_CUBE,

        ID_COUNT,
    };

    //タイトル文字アニメ用
    std::shared_ptr<Shader> m_Shader;
    std::unique_ptr<Material> m_Material;
    ResourceHandle<Texture> m_TitleTexture;

    std::vector<VERTEX_3D> m_Vertices;
    std::vector<unsigned int> m_Indices;

    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer m_IndexBuffer;


    float m_FrameTime = 0.0f;
    float m_FrameDuration = 0.08f; // アニメ速度
    int   m_CurrentFrame = 0;
    int m_SpriteCols = 10;
    int m_SpriteRows = 3;
    int m_TotalFrames = 8; // とりあえず固定


    float m_PosX = 0.0f;
    float m_PosY = -100.0f;
    float m_Width = 600.0f;
    float m_Height = 120.0f;

private:

    void SetupTitleVertices();
    void UpdateTitleUV();
    void DrawTitleText();


public:
    void Init() override;
    void UnInit() override;
    void Update() override;
    void Draw() override;
    void Draw(Camera*) override;
};