#include "ResourceManager.h"
#include "Texture.h"
#include "StaticMesh.h"
#include "Shader.h"
#include <iostream>

// ===================================================================
// 初期化
// ===================================================================
void ResourceManager::Init()
{
    std::cout << "[ResourceManager] Initialized" << std::endl;
}

// ===================================================================
// 終了処理
// ===================================================================
void ResourceManager::UnInit()
{
    std::cout << "[ResourceManager] Uninitializing..." << std::endl;
    ClearAll();
}

// ===================================================================
// テクスチャ読み込み (キャッシュ活用)
// ===================================================================
std::shared_ptr<Texture> ResourceManager::LoadTexture(const std::string& filepath)
{
    // キャッシュに存在するか確認
    auto it = m_TextureCache.find(filepath);
    if (it != m_TextureCache.end())
    {
        std::cout << "[ResourceManager] Texture cache hit: " << filepath << std::endl;
        return it->second; // キャッシュから返す
    }

    // 新規読み込み
    std::cout << "[ResourceManager] Loading texture: " << filepath << std::endl;
    auto texture = std::make_shared<Texture>();

    if (!texture->Load(filepath))
    {
        std::cerr << "[ResourceManager] Failed to load texture: " << filepath << std::endl;
        return nullptr;
    }

    // キャッシュに保存
    m_TextureCache[filepath] = texture;
    return texture;
}

// ===================================================================
// メッシュ読み込み (キャッシュ活用)
// ===================================================================
std::shared_ptr<StaticMesh> ResourceManager::LoadMesh(const std::string& filepath,
    const std::string& textureDir)
{
    // キャッシュに存在するか確認
    auto it = m_MeshCache.find(filepath);
    if (it != m_MeshCache.end())
    {
        std::cout << "[ResourceManager] Mesh cache hit: " << filepath << std::endl;
        return it->second;
    }

    // 新規読み込み
    std::cout << "[ResourceManager] Loading mesh: " << filepath << std::endl;
    auto mesh = std::make_shared<StaticMesh>();

    try
    {
        mesh->Load(filepath, textureDir);
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ResourceManager] Failed to load mesh: " << filepath
            << " - " << e.what() << std::endl;
        return nullptr;
    }

    // キャッシュに保存
    m_MeshCache[filepath] = mesh;
    return mesh;
}

// ===================================================================
// シェーダー読み込み (キャッシュ活用)
// ===================================================================
std::shared_ptr<Shader> ResourceManager::LoadShader(const std::string& vsPath,
    const std::string& psPath)
{
    // キーを生成
    std::string key = MakeShaderKey(vsPath, psPath);

    // キャッシュに存在するか確認
    auto it = m_ShaderCache.find(key);
    if (it != m_ShaderCache.end())
    {
        std::cout << "[ResourceManager] Shader cache hit: " << key << std::endl;
        return it->second;
    }

    // 新規読み込み
    std::cout << "[ResourceManager] Loading shader: " << key << std::endl;
    auto shader = std::make_shared<Shader>();

    shader->Create(vsPath, psPath);

    // キャッシュに保存
    m_ShaderCache[key] = shader;
    return shader;
}

// ===================================================================
// 全リソース解放
// ===================================================================
void ResourceManager::ClearAll()
{
    std::cout << "[ResourceManager] Clearing all caches..." << std::endl;

    m_TextureCache.clear();
    m_MeshCache.clear();
    m_ShaderCache.clear();

    std::cout << "[ResourceManager] All caches cleared" << std::endl;
}

// ===================================================================
// デバッグ情報表示
// ===================================================================
void ResourceManager::PrintCacheInfo()
{
    std::cout << "===== Resource Manager Cache Info =====" << std::endl;
    std::cout << "Textures: " << m_TextureCache.size() << std::endl;
    std::cout << "Meshes:   " << m_MeshCache.size() << std::endl;
    std::cout << "Shaders:  " << m_ShaderCache.size() << std::endl;
    std::cout << "========================================" << std::endl;
}

// ===================================================================
// シェーダーキー生成 (内部関数)
// ===================================================================
std::string ResourceManager::MakeShaderKey(const std::string& vs, const std::string& ps)
{
    return vs + "|" + ps;
}