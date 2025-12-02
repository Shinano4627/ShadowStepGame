// ===================================================================
// EmptyRender.h
// 外部リソース不要のシンプルなキューブ描画コンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Renderer.h"
#include <vector>

class EmptyRender : public MeshComponent
{
private:
    bool m_Initialized = false;

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    EmptyRender()
    {
        // レイヤーをWORLDに設定
        SetRenderLayer(RenderLayer::WORLD);
    }

    // ===================================================================
    // 初期化
    // ===================================================================
    void Init() override
    {
        using namespace DirectX::SimpleMath;

        m_Initialized = true;
    }

    // ===================================================================
    // 描画
    // ===================================================================
    void Draw(Camera* camera) override
    {
        // カメラ設定（WORLD層のみ）
        //if (camera && GetRenderLayer() == RenderLayer::WORLD)
        //{
        //    camera->SetCamera();
        //}
    }

    // ===================================================================
    // 終了処理
    // ===================================================================
    void Uninit() override
    {
    }
};