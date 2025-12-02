// ===================================================================
// GameObject.h
// コンポーネントを保持するゲームオブジェクトの基底クラス
// ===================================================================
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "Transform.h"
#include "Component.h"
#include "MeshComponent.h"
#include "Camera.h"

// ===================================================================
// GameObject クラス
// ゲーム内エンティティの基底クラス
// ===================================================================
class GameObject
{
protected:
    // ===================================================================
    // メンバ変数
    // ===================================================================
    
    // 姿勢情報 (Position, Rotation, Scale)
    Transform m_Transform;

    // コンポーネント管理
    std::vector<std::unique_ptr<Component>> m_Components;  // コンポーネントリスト
    std::vector<std::unique_ptr<MeshComponent>> m_MeshComponents;     // 3Dメッシュコンポーネント
    std::unordered_map<std::type_index, Component*> m_ComponentMap;  // 型からコンポーネントへの高速検索用
    std::unordered_map<std::type_index, MeshComponent*> m_MeshComponentMap;  // 型からコンポーネントへの高速検索用

    bool m_Active;      // GameObject が有効かどうか
    int m_id = -1;   // ID（自動設定）
    std::string m_Name = "";    // GameObject 名
    std::string m_Tag = "";      // GameObject を識別するためのタグ

public:
    // ===================================================================
    // コンストラクタ・デストラクタ
    // ===================================================================
    GameObject()
        : m_Active(true)
        , m_Tag(""){}

    GameObject(Vector3 pos, Vector3 rotate, Vector3 scale)
        : m_Active(true)
        , m_Tag("") 
        , m_Transform(pos, rotate, scale){}

    virtual ~GameObject() = default;

    // ===================================================================
    // コンポーネント管理
    // ===================================================================
    
    // コンポーネントを追加(テンプレート)
    template<typename T, typename... Args>  // <追加するコンポーネントの型,コンストラクタの引数>
    T* AddComponent(Args&&... args)         // (コンストラクタに渡す引数)
    {
        // T(追加しているコンポーネントの型)がコンポーネントを継承ししているかチェック
        static_assert(std::is_base_of<Component, T>::value,
            "T must be derived from Component");

        // 新しいコンポーネントを作成
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();

        // オーナー(this) を設定
        ptr->SetOwner(this);

        // マップに登録(型から高速検索できるように)
        m_ComponentMap[std::type_index(typeid(T))] = ptr;

        // リストに追加
        m_Components.push_back(std::move(component));

        // 初期化
        ptr->Init();

        return ptr;
    }

    // メッシュコンポーネントを追加
    template<typename T, typename... Args>  // <追加するコンポーネントの型,コンストラクタの引数>
    T* AddMeshComponent(Args&&... args)         // (コンストラクタに渡す引数)
    {
        // T(追加しているコンポーネントの型)がコンポーネントを継承ししているかチェック
        static_assert(std::is_base_of<MeshComponent, T>::value,
            "T must be derived from MeshComponent");

        // TODO:空オブジェクトがあったら削除する

        // 新しいコンポーネントを作成
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();

        // オーナー(this) を設定
        ptr->SetOwner(this);

        // マップに登録(型から高速検索できるように)
        m_MeshComponentMap[std::type_index(typeid(T))] = ptr;

        // リストに追加
        m_MeshComponents.push_back(std::move(component));

        // 初期化
        ptr->Init();

        return ptr;
    }

    // ===================================================================
    // コンポーネント取得 (テンプレート)
    // ===================================================================
    template<typename T>    // T=取得するコンポーネントの型
    T* GetComponent()
    {
        // Tがコンポーネントを継承しているかチェック
        static_assert(std::is_base_of<Component, T>::value,
            "T must be derived from Component");

        // マップから高速検索
        auto it = m_ComponentMap.find(std::type_index(typeid(T)));
        if (it != m_ComponentMap.end())
        {
            return static_cast<T*>(it->second);
        }

        return nullptr;
    }

    template<typename T>    // T=取得するコンポーネントの型
    T* GetMeshComponent()
    {
        // Tがコンポーネントを継承しているかチェック
        static_assert(std::is_base_of<MeshComponent, T>::value,
            "T must be derived from Component");

        // マップから高速検索
        auto it = m_MeshComponentMap.find(std::type_index(typeid(T)));
        if (it != m_MeshComponentMap.end())
        {
            return static_cast<T*>(it->second);
        }

        return nullptr;
    }

    // ===================================================================
    // コンポーネント取得 (const版)
    // ===================================================================
    template<typename T>
    const T* GetComponent() const
    {
        // Tがコンポーネントを継承しているかチェック
        static_assert(std::is_base_of<Component, T>::value,
            "T must be derived from Component");

        // マップから高速検索
        auto it = m_ComponentMap.find(std::type_index(typeid(T)));
        if (it != m_ComponentMap.end())
        {
            return static_cast<const T*>(it->second);
        }

        return nullptr;
    }

    template<typename T>
    const T* GetMeshComponent() const
    {
        // Tがコンポーネントを継承しているかチェック
        static_assert(std::is_base_of<MeshComponent, T>::value,
            "T must be derived from Component");

        // マップから高速検索
        auto it = m_MeshComponentMap.find(std::type_index(typeid(T)));
        if (it != m_MeshComponentMap.end())
        {
            return static_cast<const T*>(it->second);
        }

        return nullptr;
    }

    // ===================================================================
    // コンポーネント削除
    // ===================================================================
    template<typename T>
    void RemoveComponent()
    {
        // Tがコンポーネントを継承しているかチェック
        static_assert(std::is_base_of<Component, T>::value,
            "T must be derived from Component");

        // マップから削除
        auto typeIndex = std::type_index(typeid(T));
        auto it = m_ComponentMap.find(typeIndex);
        if (it != m_ComponentMap.end())
        {
            Component* compPtr = it->second;
            m_ComponentMap.erase(it);

            // リストから削除
            m_Components.erase(
                std::remove_if(m_Components.begin(), m_Components.end(),
                    [compPtr](const std::unique_ptr<Component>& comp) {
                        return comp.get() == compPtr;
                    }),
                m_Components.end()
            );
        }
    }

    template<typename T>
    void RemoveMeshComponent()
    {
        // Tがコンポーネントを継承しているかチェック
        static_assert(std::is_base_of<MeshComponent, T>::value,
            "T must be derived from Component");

        // マップから削除
        auto typeIndex = std::type_index(typeid(T));
        auto it = m_MeshComponentMap.find(typeIndex);
        if (it != m_MeshComponentMap.end())
        {
            MeshComponent* compPtr = it->second;
            m_MeshComponentMap.erase(it);

            // リストから削除
            m_MeshComponents.erase(
                std::remove_if(m_MeshComponents.begin(), m_MeshComponents.end(),
                    [compPtr](const std::unique_ptr<MeshComponent>& comp) {
                        return comp.get() == compPtr;
                    }),
                m_MeshComponents.end()
            );
        }
    }

    // ===================================================================
    // ライフサイクル
    // ===================================================================
    
    // 更新処理
    virtual void Update()
    {
        // GameObjectが無効の時は何もしない
        if (!m_Active) return;

        // 全ての有効なコンポーネントを更新
        for (auto& component : m_Components)
        {
            if (component->IsEnabled())
            {
                component->Update();
            }
        }

        for (auto& meshComponent : m_MeshComponents)
        {
            if (meshComponent->IsEnabled())
            {
                meshComponent->Update();
            }
        }
    }

    // 描画処理(全レイヤー)
    virtual void Draw(Camera* camera)
    {
        // GameObjectが無効の時は何もしない
        if (!m_Active) return;

        // すべての有効なコンポーネントを描画
        for (auto& meshComponent : m_MeshComponents)
        {
            if (meshComponent->IsEnabled() && meshComponent->IsDisplayed())
            {
                meshComponent->Draw(camera);
            }
        }
    }
    // 描画処理(レイヤー別)
    virtual void DrawLayer(Camera* camera, RenderLayer layer)
    {
        // GameObjectが無効の時は何もしない
        if (!m_Active) return;

        // 指定されたレイヤーのコンポーネントのみ描画
        for (auto& meshComponent : m_MeshComponents)
        {
            if (meshComponent->IsEnabled() && meshComponent->IsDisplayed() && meshComponent->GetRenderLayer() == layer)
            {
                meshComponent->Draw(camera);
            }
        }
    }

    // 終了処理
    virtual void Uninit()
    {
        // すべてのコンポーネントの終了処理を呼ぶ
        for (auto& component : m_Components)
        {
            component->Uninit();
        }
        for (auto& meshComponent : m_MeshComponents)
        {
            meshComponent->Uninit();
        }

        // リストとマップをクリア
        m_Components.clear();
        m_ComponentMap.clear();
        m_MeshComponents.clear();
        m_MeshComponentMap.clear();
    }

    // ===================================================================
    // Transform アクセス
    // ===================================================================

    Transform& GetTransform() { return m_Transform; }
    const Transform& GetTransform() const { return m_Transform; }

    // ===================================================================
    // アクティブ制御
    // ===================================================================

    // GameObjectを有効化/無効化
    void SetActive(bool active) { m_Active = active; }

    // GameObjectが有効かどうか取得
    bool IsActive() const { return m_Active; }

    // ===================================================================
    // オブジェクトID
    // ===================================================================
    // オブジェクトIDを設定
    void SetID(const int id) { m_id = id; }
    // オブジェクトIDを取得
    const int GetID() const { return m_id; }

    // ===================================================================
    // オブジェクト名
    // ===================================================================
    // オブジェクト名を設定
    void SetName(const std::string& name) { m_Name = name; }
    // オブジェクト名を取得
    const std::string& GetName() const { return m_Name; }

    // ===================================================================
    // タグ
    // ===================================================================
    
    // タグを設定
    void SetTag(const std::string& tag) { m_Tag = tag; }
    // タグを取得
    const std::string& GetTag() const { return m_Tag; }
};