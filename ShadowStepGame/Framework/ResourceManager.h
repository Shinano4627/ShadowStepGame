#pragma once
#include "singleton.h"
#include <memory>
#include <unordered_map>
#include <string>

// 前方宣言
class Texture;
class StaticMesh;
class Shader;

class ResourceManager
{
private:
    // キャッシュマップ (パスをキーに shared_ptr で管理)
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureCache;
    std::unordered_map<std::string, std::shared_ptr<StaticMesh>> m_MeshCache;
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderCache;

public:
    void Init();
    void UnInit();

    // ===================================================================
    // テクスチャ読み込み (キャッシュ活用)
    // ===================================================================
    std::shared_ptr<Texture> LoadTexture(const std::string& filepath);

    // ===================================================================
    // メッシュ読み込み (キャッシュ活用)
    // ===================================================================
    std::shared_ptr<StaticMesh> LoadMesh(const std::string& filepath,
        const std::string& textureDir = "");

    // ===================================================================
    // シェーダー読み込み (キャッシュ活用)
    // ===================================================================
    std::shared_ptr<Shader> LoadShader(const std::string& vsPath,
        const std::string& psPath);

    // ===================================================================
    // 全リソース解放
    // ===================================================================
    void ClearAll();

    // ===================================================================
    // デバッグ情報
    // ===================================================================
    void PrintCacheInfo();

private:
    // シェーダーキーを生成 ("vs_path|ps_path" の形式)
    std::string MakeShaderKey(const std::string& vs, const std::string& ps);
};

// 唯一インスタンス取得マクロ
#define M_RESOURCE Singleton<ResourceManager>::GetInstance()