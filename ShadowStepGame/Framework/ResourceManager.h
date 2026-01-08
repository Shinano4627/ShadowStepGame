//=======================================
// ResourceManager.h
// リソースの管理マネージャークラス
//=======================================
#pragma once
#include "singleton.h"
#include <memory>
#include <unordered_map>
#include <string>
#include "AssimpPerse.h"

// 前方宣言
class Texture;
class StaticMesh;
class Shader;
class Material;
struct MATERIAL;

//=======================================
// ResourceHandle テンプレート
//=======================================
template<typename T>
class ResourceHandle
{
private:
    std::shared_ptr<T> m_Resource;
    std::string m_Path;

public:
    ResourceHandle() = default;
    ResourceHandle(std::shared_ptr<T> resource, const std::string& path)
        : m_Resource(resource), m_Path(path) {}

    // 生ポインタ取得
    T* Get() const { return m_Resource.get(); }

    // operator-> はshared_ptrに委譲（完全な型定義が必要）
    T* operator->() const { return m_Resource.get(); }

    // bool変換
    explicit operator bool() const { return m_Resource != nullptr; }

    // パス取得
    const std::string& GetPath() const { return m_Path; }

    // shared_ptr取得
    std::shared_ptr<T> GetShared() const { return m_Resource; }
};

//=======================================
// ResourceManager クラス
//=======================================
class ResourceManager
{
private:
    //=======================================
    // リソースキャッシュマップ
    //=======================================
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_TextureCache;
    std::unordered_map<std::string, std::shared_ptr<StaticMesh>> m_MeshCache;
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderCache;
    std::unordered_map<std::string, std::shared_ptr<Material>> m_MaterialCache;
    std::unordered_map<std::string, const aiScene*> m_AnimationCache;

    //=======================================
    // デフォルトリソース（フォールバック用）
    //=======================================
    std::shared_ptr<Texture> m_DefaultTexture;
    std::shared_ptr<Shader> m_DefaultShader;
    std::shared_ptr<Material> m_DefaultMaterial;

    //=======================================
    // リソース統計情報
    //=======================================
    struct ResourceStats
    {
        size_t textureCount = 0;
        size_t meshCount = 0;
        size_t shaderCount = 0;
        size_t materialCount = 0;
        size_t totalMemoryUsage = 0; // バイト単位（将来実装）
    };
    ResourceStats m_Stats;

    //=======================================
    // スレッドセーフティ
    //=======================================
    std::mutex m_TextureMutex;
    std::mutex m_MeshMutex;
    std::mutex m_ShaderMutex;
    std::mutex m_MaterialMutex;

    //=======================================
    // デフォルトリソースパス
    //=======================================
    const std::string DEFAULT_TEXTURE_PATH = "asset/texture/default.png";
    const std::string DEFAULT_SHADER_VS_PATH = "shader/litTextureVS.hlsl";
    const std::string DEFAULT_SHADER_PS_PATH = "shader/litTexturePS.hlsl";

public:
    void Init();
    void UnInit();

    //=======================================
    // テクスチャ読み込み (キャッシュ活用)
    //=======================================
    ResourceHandle<Texture> LoadTexture(const std::string& filepath);

    // テクスチャを強制再読み込み
    ResourceHandle<Texture> ReloadTexture(const std::string& filepath);

    // デフォルトテクスチャ取得
    ResourceHandle<Texture> GetDefaultTexture();

    //=======================================
    // メッシュ読み込み (キャッシュ活用)
    //=======================================
    ResourceHandle<StaticMesh> LoadMesh(
        const std::string& filepath,
        const std::string& textureDir = "");

    // メッシュを強制再読み込み
    ResourceHandle<StaticMesh> ReloadMesh(
        const std::string& filepath,
        const std::string& textureDir = "");

    //=======================================
    // シェーダー読み込み (キャッシュ活用)
    //=======================================
    ResourceHandle<Shader> LoadShader(
        const std::string& vsPath,
        const std::string& psPath);

    // シェーダーを強制再読み込み
    ResourceHandle<Shader> ReloadShader(
        const std::string& vsPath,
        const std::string& psPath);

    // デフォルトシェーダー取得
    ResourceHandle<Shader> GetDefaultShader();

    //=======================================
    // マテリアル作成（キャッシュ活用）
    //=======================================
    ResourceHandle<Material> CreateMaterial(
        const std::string& name,
        const MATERIAL& materialData);

    // デフォルトマテリアル取得
    ResourceHandle<Material> GetDefaultMaterial();
    // ===================================================================
    // アニメーション読み込み (キャッシュ活用)
    // ===================================================================
    void LoadAnimation(std::weak_ptr<StaticMesh>mesh, const char* filepath, const char* name, bool flip);

    //=======================================
    // リソース存在チェック
    //=======================================
    bool HasTexture(const std::string& filepath) const;
    bool HasMesh(const std::string& filepath) const;
    bool HasShader(const std::string& vsPath, const std::string& psPath) const;
    bool HasMaterial(const std::string& name) const;

    //=======================================
    // リソース解放
    //=======================================
    void UnloadTexture(const std::string& filepath);
    void UnloadMesh(const std::string& filepath);
    void UnloadShader(const std::string& vsPath, const std::string& psPath);
    void UnloadMaterial(const std::string& name);

    // 特定タイプのリソースをすべて解放
    void UnloadAllTextures();
    void UnloadAllMeshes();
    void UnloadAllShaders();
    void UnloadAllMaterials();
    // ===================================================================
    // 全リソース解放
    // ===================================================================
    void ClearAll();

    //=======================================
    // リソース情報
    //=======================================
    const ResourceStats& GetStats() const { return m_Stats; }
    void PrintCacheInfo() const;
    void PrintDetailedInfo() const;

    //=======================================
    // リソースリスト取得（デバッグ用）
    //=======================================
    std::vector<std::string> GetLoadedTextureList() const;
    std::vector<std::string> GetLoadedMeshList() const;
    std::vector<std::string> GetLoadedShaderList() const;
    std::vector<std::string> GetLoadedMaterialList() const;

private:
    //=======================================
    // 内部関数
    //=======================================
    
    // シェーダーキーを生成 ("vs_path|ps_path" の形式)
    std::string MakeShaderKey(const std::string& vs, const std::string& ps) const;

    // パス正規化（区切り文字統一など）
    std::string NormalizePath(const std::string& path) const;

    // デフォルトリソースを作成
    void CreateDefaultResources();

    // 統計情報を更新
    void UpdateStats();
};

// 唯一インスタンス取得マクロ
#define M_RESOURCE Singleton<ResourceManager>::GetInstance()