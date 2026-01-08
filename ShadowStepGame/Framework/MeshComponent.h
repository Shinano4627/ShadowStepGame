#pragma once
// ===================================================================
// MeshComponent.h
// GameObject に追加できる機能の基底クラス
// ===================================================================
#pragma once
#include "SystemCommon.h"
#include "Component.h"
#include "Shader.h"
#include "Renderer.h"

// ===================================================================
// 前方宣言
// ===================================================================
class GameObject;
class Camera;

// ===================================================================
// Component 基底クラス
// すべてのコンポーネントはこのクラスを継承する
// ===================================================================
class MeshComponent : public Component
{
protected:
    // ===================================================================
    // メンバ変数
    // ===================================================================
    bool m_Displayed = true;            // メッシュを表示するか
    RenderLayer m_RenderLayer = RenderLayer::WORLD; // 描画レイヤー
    DirectX::SimpleMath::Color m_Color;
    bool m_DispOutline = false;
    std::shared_ptr<Shader> m_ShaderOutline;
public:
    // ===================================================================
    // コンストラクタ・デストラクタ
    // ===================================================================
    MeshComponent(const DirectX::SimpleMath::Color& color = DirectX::SimpleMath::Color(1, 1, 1, 1))
        : m_RenderLayer(RenderLayer::WORLD)
        , m_Color(color)
    {
        MakeShaderOutline();
        m_ShaderOutline->SetRasterizeState(RS_CULLFRONT);   // 裏面だけ描画
    }

    virtual ~MeshComponent() = default;

    // ===================================================================
    // ライフサイクル (派生クラスでオーバーライド)
    // ===================================================================
    // 描画処理
    virtual void Draw(Camera* camera) {}

    // ===================================================================
    // 表示・非表示の制御
    // ===================================================================

    // コンポーネントの表示状態を変更
    void SetDisplayed(bool enabled) { m_Displayed = enabled; }

    // コンポーネントが表示状態かどうか
    bool IsDisplayed() const { return m_Displayed; }

    // ===================================================================
    // 描画レイヤー制御（追加）
    // ===================================================================

    // 描画レイヤーを設定
    void SetRenderLayer(RenderLayer layer) { m_RenderLayer = layer; }

    // 描画レイヤーを取得
    RenderLayer GetRenderLayer() const { return m_RenderLayer; }

    // ===================================================================
    // 色設定 (派生クラスでオーバーライド)
    // ===================================================================
    //virtual void SetColor(const DirectX::SimpleMath::Color& color) {}

    DirectX::SimpleMath::Color GetColor() { return m_Color; };

    // ===================================================================
    // アウトライン
    // ===================================================================
    void MakeShaderOutline()
    {
        // シェーダー作成
        m_ShaderOutline = std::make_shared<Shader>();
        m_ShaderOutline->Create("shader/outlineShaderVS.hlsl", "shader/outlineShaderPS.hlsl");
    }
    void SetDispOutline(bool enabled) { m_DispOutline = enabled; };
    bool IsDispOutline() { return m_DispOutline; };
};