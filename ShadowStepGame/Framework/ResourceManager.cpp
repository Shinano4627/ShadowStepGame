#include "ResourceManager.h"
#include "Texture.h"
#include "StaticMesh.h"
#include "Shader.h"
#include "Material.h"
#include "Renderer.h"
#include <iostream>

// ===================================================================
// 初期化
// ===================================================================
void ResourceManager::Init()
{
    std::cout << "[ResourceManager] Initialized..." << std::endl;
    
    // デフォルトリソースを作成
    CreateDefaultResources();

    // 統計情報を初期化
    UpdateStats();

    std::cout << "[ResourceManager] Initialized successfully" << std::endl;
}

// ===================================================================
// 終了処理
// ===================================================================
void ResourceManager::UnInit()
{
    std::cout << "[ResourceManager] Shutting down..." << std::endl;

    // 統計情報を表示
    PrintCacheInfo();

    // すべてのリソースを解放
    ClearAll();

    std::cout << "[ResourceManager] Shutdown complete" << std::endl;
}

// ===================================================================
// テクスチャ読み込み（キャッシュ活用）
// ===================================================================
ResourceHandle<Texture> ResourceManager::LoadTexture(const std::string& filepath)
{
    if (filepath.empty())
    {
        std::cout << "[ResourceManager] Empty texture path, returning default texture" << std::endl;
        return GetDefaultTexture();
    }

    // パス正規化
    std::string normalizedPath = NormalizePath(filepath);

    // スレッドセーフなキャッシュチェック
    {
        std::lock_guard<std::mutex> lock(m_TextureMutex);

        // キャッシュに存在するか確認
        auto it = m_TextureCache.find(normalizedPath);
        if (it != m_TextureCache.end())
        {
            std::cout << "[ResourceManager] Texture cache hit: " << normalizedPath << std::endl;
            return ResourceHandle<Texture>(it->second, normalizedPath);
        }
    }

    // 新規読み込み
    std::cout << "[ResourceManager] Loading texture: " << normalizedPath << std::endl;
    auto texture = std::make_shared<Texture>();

    if (!texture->Load(normalizedPath))
    {
        std::cerr << "[ResourceManager] Failed to load texture: " << normalizedPath << std::endl;
        std::cerr << "[ResourceManager] Using default texture as fallback" << std::endl;
        return GetDefaultTexture();
    }

    // キャッシュに保存（スレッドセーフ）
    {
        std::lock_guard<std::mutex> lock(m_TextureMutex);
        m_TextureCache[normalizedPath] = texture;
        UpdateStats();
    }

    return ResourceHandle<Texture>(texture, normalizedPath);
}

// ===================================================================
// テクスチャ強制再読み込み
// ===================================================================
ResourceHandle<Texture> ResourceManager::ReloadTexture(const std::string& filepath)
{
    std::string normalizedPath = NormalizePath(filepath);

    std::cout << "[ResourceManager] Reloading texture: " << normalizedPath << std::endl;

    // キャッシュから削除
    {
        std::lock_guard<std::mutex> lock(m_TextureMutex);
        m_TextureCache.erase(normalizedPath);
    }

    // 再読み込み
    return LoadTexture(filepath);
}

// ===================================================================
// デフォルトテクスチャ取得
// ===================================================================
ResourceHandle<Texture> ResourceManager::GetDefaultTexture()
{
    return ResourceHandle<Texture>(m_DefaultTexture, DEFAULT_TEXTURE_PATH);
}

// ===================================================================
// メッシュ読み込み（キャッシュ活用）
// ===================================================================
ResourceHandle<StaticMesh> ResourceManager::LoadMesh(
    const std::string& filepath,
    const std::string& textureDir)
{
    if (filepath.empty())
    {
        std::cerr << "[ResourceManager] Empty mesh path provided" << std::endl;
        return ResourceHandle<StaticMesh>();
    }

    // パス正規化
    std::string normalizedPath = NormalizePath(filepath);

    // スレッドセーフなキャッシュチェック
    {
        std::lock_guard<std::mutex> lock(m_MeshMutex);

        auto it = m_MeshCache.find(normalizedPath);
        if (it != m_MeshCache.end())
        {
            std::cout << "[ResourceManager] Mesh cache hit: " << normalizedPath << std::endl;
            return ResourceHandle<StaticMesh>(it->second, normalizedPath);
        }
    }

    // 新規読み込み
    std::cout << "[ResourceManager] Loading mesh: " << normalizedPath << std::endl;
    auto mesh = std::make_shared<StaticMesh>();

    try
    {
        mesh->Load(normalizedPath, textureDir);
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ResourceManager] Failed to load mesh: " << normalizedPath
            << " - " << e.what() << std::endl;
        return ResourceHandle<StaticMesh>();
    }

    // キャッシュに保存（スレッドセーフ）
    {
        std::lock_guard<std::mutex> lock(m_MeshMutex);
        m_MeshCache[normalizedPath] = mesh;
        UpdateStats();
    }

    return ResourceHandle<StaticMesh>(mesh, normalizedPath);
}

// ===================================================================
// メッシュ強制再読み込み
// ===================================================================
ResourceHandle<StaticMesh> ResourceManager::ReloadMesh(
    const std::string& filepath,
    const std::string& textureDir)
{
    std::string normalizedPath = NormalizePath(filepath);

    std::cout << "[ResourceManager] Reloading mesh: " << normalizedPath << std::endl;

    // キャッシュから削除
    {
        std::lock_guard<std::mutex> lock(m_MeshMutex);
        m_MeshCache.erase(normalizedPath);
    }

    // 再読み込み
    return LoadMesh(filepath, textureDir);
}

// ===================================================================
// シェーダー読み込み（キャッシュ活用）
// ===================================================================
ResourceHandle<Shader> ResourceManager::LoadShader(
    const std::string& vsPath,
    const std::string& psPath)
{
    // キーを生成
    std::string key = MakeShaderKey(vsPath, psPath);

    // スレッドセーフなキャッシュチェック
    {
        std::lock_guard<std::mutex> lock(m_ShaderMutex);

        auto it = m_ShaderCache.find(key);
        if (it != m_ShaderCache.end())
        {
            std::cout << "[ResourceManager] Shader cache hit: " << key << std::endl;
            return ResourceHandle<Shader>(it->second, key);
        }
    }

    // 新規読み込み
    std::cout << "[ResourceManager] Loading shader: " << key << std::endl;
    auto shader = std::make_shared<Shader>();

    shader->Create(vsPath, psPath);

    // キャッシュに保存（スレッドセーフ）
    {
        std::lock_guard<std::mutex> lock(m_ShaderMutex);
        m_ShaderCache[key] = shader;
        UpdateStats();
    }

    return ResourceHandle<Shader>(shader, key);
}

// ===================================================================
// シェーダー強制再読み込み
// ===================================================================
ResourceHandle<Shader> ResourceManager::ReloadShader(
    const std::string& vsPath,
    const std::string& psPath)
{
    std::string key = MakeShaderKey(vsPath, psPath);

    std::cout << "[ResourceManager] Reloading shader: " << key << std::endl;

    // キャッシュから削除
    {
        std::lock_guard<std::mutex> lock(m_ShaderMutex);
        m_ShaderCache.erase(key);
    }

    // 再読み込み
    return LoadShader(vsPath, psPath);
}

// ===================================================================
// デフォルトシェーダー取得
// ===================================================================
ResourceHandle<Shader> ResourceManager::GetDefaultShader()
{
    std::string key = MakeShaderKey(DEFAULT_SHADER_VS_PATH, DEFAULT_SHADER_PS_PATH);
    return ResourceHandle<Shader>(m_DefaultShader, key);
}

// ===================================================================
// マテリアル作成（キャッシュ活用）
// ===================================================================
ResourceHandle<Material> ResourceManager::CreateMaterial(
    const std::string& name,
    const MATERIAL& materialData)
{
    if (name.empty())
    {
        std::cerr << "[ResourceManager] Empty material name provided" << std::endl;
        return GetDefaultMaterial();
    }

    // スレッドセーフなキャッシュチェック
    {
        std::lock_guard<std::mutex> lock(m_MaterialMutex);

        auto it = m_MaterialCache.find(name);
        if (it != m_MaterialCache.end())
        {
            std::cout << "[ResourceManager] Material cache hit: " << name << std::endl;
            return ResourceHandle<Material>(it->second, name);
        }
    }

    // 新規作成
    std::cout << "[ResourceManager] Creating material: " << name << std::endl;
    auto material = std::make_shared<Material>(materialData);

    // キャッシュに保存（スレッドセーフ）
    {
        std::lock_guard<std::mutex> lock(m_MaterialMutex);
        m_MaterialCache[name] = material;
        UpdateStats();
    }

    return ResourceHandle<Material>(material, name);
}

// ===================================================================
// デフォルトマテリアル取得
// ===================================================================
ResourceHandle<Material> ResourceManager::GetDefaultMaterial()
{
    return ResourceHandle<Material>(m_DefaultMaterial, "DefaultMaterial");
}

// ===================================================================
// アニメーション読み込み (キャッシュ活用)
// ===================================================================
void ResourceManager::LoadAnimation(std::weak_ptr<StaticMesh>mesh, const char* filepath, const char* name, bool flip)
{
    const aiScene* anim = nullptr;

    // キャッシュに存在するか確認
    auto it = m_AnimationCache.find(filepath);
    if (it != m_AnimationCache.end())
    {
        std::cout << "[ResourceManager] Animation cache hit: " << filepath << std::endl;
        mesh.lock()->LoadAnimation(it->second, filepath, name, flip);
        return;
    }

    // 新規読み込み
    std::cout << "[ResourceManager] Loading Animation: " << filepath << std::endl;
    try
    {
        anim = mesh.lock()->LoadAnimation(anim, filepath, name, flip);
    }
    catch (const std::exception& e)
    {
        std::cerr << "[ResourceManager] Failed to load mesh: " << filepath
            << " - " << e.what() << std::endl;
        return;
    }

    // キャッシュに保存
    m_AnimationCache[filepath] = anim;
}
// ===================================================================
// リソース存在チェック
// ===================================================================
bool ResourceManager::HasTexture(const std::string& filepath) const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_TextureMutex));
    return m_TextureCache.find(NormalizePath(filepath)) != m_TextureCache.end();
}

bool ResourceManager::HasMesh(const std::string& filepath) const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_MeshMutex));
    return m_MeshCache.find(NormalizePath(filepath)) != m_MeshCache.end();
}

bool ResourceManager::HasShader(const std::string& vsPath, const std::string& psPath) const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_ShaderMutex));
    return m_ShaderCache.find(MakeShaderKey(vsPath, psPath)) != m_ShaderCache.end();
}

bool ResourceManager::HasMaterial(const std::string& name) const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_MaterialMutex));
    return m_MaterialCache.find(name) != m_MaterialCache.end();
}

// ===================================================================
// リソース解放
// ===================================================================
void ResourceManager::UnloadTexture(const std::string& filepath)
{
    std::lock_guard<std::mutex> lock(m_TextureMutex);
    std::string normalizedPath = NormalizePath(filepath);

    if (m_TextureCache.erase(normalizedPath) > 0)
    {
        std::cout << "[ResourceManager] Unloaded texture: " << normalizedPath << std::endl;
        UpdateStats();
    }
}

void ResourceManager::UnloadMesh(const std::string& filepath)
{
    std::lock_guard<std::mutex> lock(m_MeshMutex);
    std::string normalizedPath = NormalizePath(filepath);

    if (m_MeshCache.erase(normalizedPath) > 0)
    {
        std::cout << "[ResourceManager] Unloaded mesh: " << normalizedPath << std::endl;
        UpdateStats();
    }
}

void ResourceManager::UnloadShader(const std::string& vsPath, const std::string& psPath)
{
    std::lock_guard<std::mutex> lock(m_ShaderMutex);
    std::string key = MakeShaderKey(vsPath, psPath);

    if (m_ShaderCache.erase(key) > 0)
    {
        std::cout << "[ResourceManager] Unloaded shader: " << key << std::endl;
        UpdateStats();
    }
}

void ResourceManager::UnloadMaterial(const std::string& name)
{
    std::lock_guard<std::mutex> lock(m_MaterialMutex);

    if (m_MaterialCache.erase(name) > 0)
    {
        std::cout << "[ResourceManager] Unloaded material: " << name << std::endl;
        UpdateStats();
    }
}

void ResourceManager::UnloadAllTextures()
{
    std::lock_guard<std::mutex> lock(m_TextureMutex);
    m_TextureCache.clear();
    std::cout << "[ResourceManager] All textures unloaded" << std::endl;
    UpdateStats();
}

void ResourceManager::UnloadAllMeshes()
{
    std::lock_guard<std::mutex> lock(m_MeshMutex);
    m_MeshCache.clear();
    std::cout << "[ResourceManager] All meshes unloaded" << std::endl;
    UpdateStats();
}

void ResourceManager::UnloadAllShaders()
{
    std::lock_guard<std::mutex> lock(m_ShaderMutex);
    m_ShaderCache.clear();
    m_AnimationCache.clear();
    std::cout << "[ResourceManager] All shaders unloaded" << std::endl;
    UpdateStats();
}

void ResourceManager::UnloadAllMaterials()
{
    std::lock_guard<std::mutex> lock(m_MaterialMutex);
    m_MaterialCache.clear();
    std::cout << "[ResourceManager] All materials unloaded" << std::endl;
    UpdateStats();
}

void ResourceManager::ClearAll()
{
    std::cout << "[ResourceManager] Clearing all caches..." << std::endl;

    UnloadAllTextures();
    UnloadAllMeshes();
    UnloadAllShaders();
    UnloadAllMaterials();

    // デフォルトリソースもクリア
    m_DefaultTexture.reset();
    m_DefaultShader.reset();
    m_DefaultMaterial.reset();

    std::cout << "[ResourceManager] All caches cleared" << std::endl;
}

// ===================================================================
// 統計情報表示
// ===================================================================
void ResourceManager::PrintCacheInfo() const
{
    std::cout << "===== Resource Manager Cache Info =====" << std::endl;
    std::cout << "Textures:  " << m_Stats.textureCount << std::endl;
    std::cout << "Meshes:    " << m_Stats.meshCount << std::endl;
    std::cout << "Shaders:   " << m_Stats.shaderCount << std::endl;
    std::cout << "Materials: " << m_Stats.materialCount << std::endl;
    std::cout << "========================================" << std::endl;
}

void ResourceManager::PrintDetailedInfo() const
{
    std::cout << "===== Resource Manager Detailed Info =====" << std::endl;

    std::cout << "\n[Textures] (" << m_Stats.textureCount << ")" << std::endl;
    for (const auto& pair : m_TextureCache)
    {
        std::cout << "  - " << pair.first
            << " (refs: " << pair.second.use_count() << ")" << std::endl;
    }

    std::cout << "\n[Meshes] (" << m_Stats.meshCount << ")" << std::endl;
    for (const auto& pair : m_MeshCache)
    {
        std::cout << "  - " << pair.first
            << " (refs: " << pair.second.use_count() << ")" << std::endl;
    }

    std::cout << "\n[Shaders] (" << m_Stats.shaderCount << ")" << std::endl;
    for (const auto& pair : m_ShaderCache)
    {
        std::cout << "  - " << pair.first
            << " (refs: " << pair.second.use_count() << ")" << std::endl;
    }

    std::cout << "\n[Materials] (" << m_Stats.materialCount << ")" << std::endl;
    for (const auto& pair : m_MaterialCache)
    {
        std::cout << "  - " << pair.first
            << " (refs: " << pair.second.use_count() << ")" << std::endl;
    }

    std::cout << "===========================================" << std::endl;
}

// ===================================================================
// リソースリスト取得
// ===================================================================
std::vector<std::string> ResourceManager::GetLoadedTextureList() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_TextureMutex));

    std::vector<std::string> result;
    result.reserve(m_TextureCache.size());

    for (const auto& pair : m_TextureCache)
    {
        result.push_back(pair.first);
    }

    return result;
}

std::vector<std::string> ResourceManager::GetLoadedMeshList() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_MeshMutex));

    std::vector<std::string> result;
    result.reserve(m_MeshCache.size());

    for (const auto& pair : m_MeshCache)
    {
        result.push_back(pair.first);
    }

    return result;
}

std::vector<std::string> ResourceManager::GetLoadedShaderList() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_ShaderMutex));

    std::vector<std::string> result;
    result.reserve(m_ShaderCache.size());

    for (const auto& pair : m_ShaderCache)
    {
        result.push_back(pair.first);
    }

    return result;
}

std::vector<std::string> ResourceManager::GetLoadedMaterialList() const
{
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(m_MaterialMutex));

    std::vector<std::string> result;
    result.reserve(m_MaterialCache.size());

    for (const auto& pair : m_MaterialCache)
    {
        result.push_back(pair.first);
    }

    return result;
}

// ===================================================================
// 内部ヘルパー関数
// ===================================================================

// シェーダーキー生成
std::string ResourceManager::MakeShaderKey(const std::string& vs, const std::string& ps) const
{
    return vs + "|" + ps;
}

// パス正規化
std::string ResourceManager::NormalizePath(const std::string& path) const
{
    std::string normalized = path;

    // バックスラッシュをスラッシュに統一
    std::replace(normalized.begin(), normalized.end(), '\\', '/');

    // 連続するスラッシュを1つに
    auto newEnd = std::unique(normalized.begin(), normalized.end(),
        [](char a, char b) { return a == '/' && b == '/'; });
    normalized.erase(newEnd, normalized.end());

    return normalized;
}

// デフォルトリソース作成
void ResourceManager::CreateDefaultResources()
{
    std::cout << "[ResourceManager] Creating default resources..." << std::endl;

    // デフォルトテクスチャ（1x1の白ピクセル）
    m_DefaultTexture = std::make_shared<Texture>();
    // NOTE: 実際にはデフォルトテクスチャファイルを読み込むか、
    // プログラム内で1x1の白テクスチャを生成する
    // ここでは簡略化のため省略

    // デフォルトシェーダー
    m_DefaultShader = std::make_shared<Shader>();
    m_DefaultShader->Create(DEFAULT_SHADER_VS_PATH, DEFAULT_SHADER_PS_PATH);

    // デフォルトマテリアル
    MATERIAL defaultMtrl{};
    defaultMtrl.Ambient = DirectX::SimpleMath::Color(1, 1, 1, 1);
    defaultMtrl.Diffuse = DirectX::SimpleMath::Color(1, 1, 1, 1);
    defaultMtrl.Specular = DirectX::SimpleMath::Color(0, 0, 0, 0);
    defaultMtrl.Emission = DirectX::SimpleMath::Color(0, 0, 0, 0);
    defaultMtrl.Shininess = 0.0f;
    defaultMtrl.TextureEnable = FALSE;

    m_DefaultMaterial = std::make_shared<Material>(defaultMtrl);

    std::cout << "[ResourceManager] Default resources created" << std::endl;
}

// 統計情報更新
void ResourceManager::UpdateStats()
{
    m_Stats.textureCount = m_TextureCache.size();
    m_Stats.meshCount = m_MeshCache.size();
    m_Stats.shaderCount = m_ShaderCache.size();
    m_Stats.materialCount = m_MaterialCache.size();

    // TODO: メモリ使用量の計算（将来実装）
    m_Stats.totalMemoryUsage = 0;
}