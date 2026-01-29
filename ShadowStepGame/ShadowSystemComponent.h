//=======================================
// ShadowSystemComponent.h
// すべての影を管理
//=======================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "GameObjectList.h"
#include "IOManager.h"
#include "Game.h"
#include <algorithm>
#include <vector>

// コンポーネント
#include "SimplePlaneRendererComponent.h"
#include "Texture2D.h"

struct ShadowParam
{
    MapPosition lightDirection = {0,0};
    int length;    // 影の長さ
};

class ShadowSystemComponent : public Component
{
private:
    // 影マップ（0:影なし,1:影あり）
    int** m_ShadowMapData = nullptr; 
    // 影の元オブジェクトIDマップ（0:なし）
    int** m_ShadowSourceMap = nullptr;

    int m_MapWidth = 0;
    int m_MapHeight = 0;

    // 描画用パラメータ
    float m_SizePiece = 0.0f;
    float m_DrawStartPosX = 0.0f;
    float m_DrawStartPosZ = 0.0f;

    // 影オブジェクトの高さ（マップタイルより少し上）
    const float m_ShadowHeight = 0.5f;

    // 影のテクスチャ（xmlから取得）
    std::string m_ShadowTexturePath = "";

    // 影オブジェクト用ID 開始値5000
    int m_LastObjectId = 5000;

public:
    //=======================================
    // コンストラクタ
    //=======================================
    ShadowSystemComponent(){}

    //=======================================
    // デストラクタ
    //=======================================
    ~ShadowSystemComponent()
    {
        for (int z = 0; z < m_MapHeight; ++z)
        {
            delete[] m_ShadowMapData[z];
        }
        delete[] m_ShadowMapData;
    }

    //=======================================
    // 初期化
    //=======================================
    void Init(){}
    void SetUp(int mapWidth, int mapHeight, float sizePiece, float startPosX, float startPosZ,
               const int* const* mapData, GameObjectList* gameObjectList)
    {
        m_MapWidth = mapWidth;
        m_MapHeight = mapHeight;
        m_SizePiece = sizePiece;
        m_DrawStartPosX = startPosX;
        m_DrawStartPosZ = startPosZ;

        MakeShadowMap();
        CreateShadowObjects(mapData, gameObjectList);
    }

    //=======================================
    // 更新
    //=======================================
    void Update() override {}

    //=======================================
    // 影計算
    //=======================================
    void UpdateShadowMap(
        const int* const* mapData,
        const ShadowParam& param,
        GameObjectList* gameObjectList
    )
    {
        //=======================================
        // 初期化
        //=======================================
        ClearShadowMap();

        //=======================================
        // データ更新
        //=======================================
        // ★TODO　巨人など影の大きさが異なるものにも対応する
        for (int z = 0; z < m_MapHeight; ++z)
        {
            for (int x = 0; x < m_MapWidth; ++x)
            {
                // オブジェクトがある場合のみ
                if (mapData[z][x] == 0 ||
                    mapData[z][x] == 5 ||
                    mapData[z][x] == 99) continue;

                int shadowX = x;
                int shadowZ = z;

                //-----------------------------------
                // 一律方向・一律長さで影を生成
                //-----------------------------------
                for (int i = 0; i < param.length; ++i)
                {
                    shadowX += param.lightDirection.x;
                    shadowZ += param.lightDirection.z;

                    // マップ外なら終了
                    if (shadowX < 0 || shadowX >= m_MapWidth ||
                        shadowZ < 0 || shadowZ >= m_MapHeight)
                        break;

                    // 空きマスのみ影
                    if (mapData[shadowZ][shadowX] == 0)
                    {
                        m_ShadowMapData[shadowZ][shadowX] = 1;
                        m_ShadowSourceMap[shadowZ][shadowX] = mapData[z][x]; // 元オブジェクトtypeを入れる
                    }
                }
            }
        }

        //=======================================
        // 影オブジェクトの表示/Scale/Positionを更新
        //=======================================
        UpdateShadowObjects(mapData, param, gameObjectList);
    }

    //=======================================
    // 影の大きさ・向き
    //=======================================
    ShadowParam CalcShadowParm(
        MapPosition lightDirection,
        int mapWidth,int mapHeight
    )
    {
        ShadowParam param{};

        param.lightDirection = lightDirection;

        //=======================================
        // 真上（影なし）
        //=======================================
        if (lightDirection.x == 0 && lightDirection.z == 0)
        {
            param.length = 0;
            return param;
        }

        //=======================================
        // 影方向（十字4方向）
        //=======================================


        //=======================================
        // 影の長さ（距離感）
        //=======================================
        int distance =
            abs(lightDirection.x) + abs(lightDirection.z);   // マンハッタン距離

        param.length = std::clamp(
            distance / 2,   // SHADOW_DIV
            2,              // MIN_SHADOW
            8               // MAX_SHADOW
        );

        return param;
    }

    //=======================================
    // 取得
    //=======================================
    const int* const* GetShadowMap() const
    { return m_ShadowMapData;};
    // 影の元IDを取得
    int GetShadowSourceAt(int mapX, int mapZ) const
    {
        if (mapX < 0 || mapX >= m_MapWidth || mapZ < 0 || mapZ >= m_MapHeight) return 0;
        return m_ShadowSourceMap[mapZ][mapX];
    }

private:
    //=======================================
    // 影マップ生成
    //=======================================
    void MakeShadowMap()
    {
        m_ShadowMapData = new int* [m_MapHeight];
        m_ShadowSourceMap = new int* [m_MapHeight];
        for (int z = 0; z < m_MapHeight; ++z)
        {
            m_ShadowMapData[z] = new int[m_MapWidth];
            m_ShadowSourceMap[z] = new int[m_MapWidth];
        }
        ClearShadowMap();
    }

    //=======================================
    // 初期化
    //=======================================
    void ClearShadowMap()
    {
        for (int z = 0; z < m_MapHeight; ++z)
        {
            for (int x = 0; x < m_MapWidth; ++x)
            {
                m_ShadowMapData[z][x] = 0;
                m_ShadowSourceMap[z][x] = 0;
            }
        }
    }

    //=======================================
    // 影オブジェクト生成（MapDataが0以外のセルのみ）
    //=======================================
    void CreateShadowObjects(const int* const* mapData, GameObjectList* gameObjectList)
    {
        if (!gameObjectList) return;
        if (!mapData) return;

        // ShadowTemplateを取得
        GameObject* shadowTemplate = gameObjectList->FindGameObjectWithTag("ShadowTemplate");
        if (!shadowTemplate)
        {
            std::cout << "[ShadowSystem] ShadowTemplateが見つかりません！" << std::endl;
            return;
        }

        // テンプレートからテクスチャパスを取得
        auto* templateRenderer = shadowTemplate->GetMeshComponent<Texture2D>();
        if (templateRenderer)
        {
            m_ShadowTexturePath = templateRenderer->GetTexturePath();
        }

        // テンプレートを非表示
        shadowTemplate->SetActive(false);

        // MapDataが0以外のセルに対して影オブジェクトを生成        
        int cnt = 0;

        for (int z = 0; z < m_MapHeight; ++z)
        {
            for (int x = 0; x < m_MapWidth; ++x)
            {
                // 0の場合は生成しない
                if (mapData[z][x] == 0)
                    continue;

                // 影を落とすオブジェクト（壁=1、樹=4）のみ
                if (mapData[z][x] != 1 && mapData[z][x] != 4)
                    continue;

                // 元オブジェクトの位置計算
                float posX = m_DrawStartPosX + x * m_SizePiece;
                float posZ = m_DrawStartPosZ + z * m_SizePiece;

                // オブジェクト生成（初期状態では基本サイズ）
                auto obj = std::make_unique<GameObject>(
                    Vector3(posX, m_ShadowHeight, posZ),
                    Vector3::Zero,
                    Vector3(m_SizePiece, 1.0f, m_SizePiece)
                );
                GameObject* newObject = obj.get();
                newObject->SetID(m_LastObjectId++);
                newObject->SetName("Shadow");
                newObject->SetTag("Shadow");

                // レンダラー追加（SimplePlaneRendererComponentは3D用）
                Color shadowColor = Color(0.0f, 0.0f, 0.0f, 0.5f);
                newObject->AddMeshComponent<SimplePlaneRendererComponent>(shadowColor, m_ShadowTexturePath);

                gameObjectList->AddObject(std::move(obj));
                cnt++;
            }
        }

        std::cout << "[ShadowSystem] 影オブジェクトを" << cnt << "個生成しました" << std::endl;
    }

    //=======================================
    // 影オブジェクトの表示/Scale/Positionを更新
    //=======================================
    void UpdateShadowObjects(const int* const* mapData, const ShadowParam& param, GameObjectList* gameObjectList)
    {
        // 影の方向（光源の反対方向）
        int shadowDirX = -param.lightDirection.x;
        int shadowDirZ = -param.lightDirection.z;

        int shadowCnt = 0;

        std::vector<GameObject*> shadowObjects = gameObjectList->FindGameObjectsWithTag("Shadow");
        // すべて非表示
        for (auto& object : shadowObjects)
        {
            object->SetActive(false);
        }

        for (int z = 0; z < m_MapHeight; ++z)
        {
            for (int x = 0; x < m_MapWidth; ++x)
            {
                // オブジェクトありの場合
                if (mapData[z][x] != 0)
                {
                    // 影がオブジェクトが足りない場合は追加
                    if (shadowObjects.size() < shadowCnt)
                    {
                        // オブジェクト生成（初期状態では基本サイズ）
                        auto obj = std::make_unique<GameObject>(
                            Vector3(0.f, m_ShadowHeight, 0.f),  // 位置はあとで変えるので仮
                            Vector3::Zero,
                            Vector3(m_SizePiece, 1.0f, m_SizePiece)
                        );
                        GameObject* newObject = obj.get();
                        newObject->SetID(m_LastObjectId++);
                        newObject->SetName("Shadow");
                        newObject->SetTag("Shadow");

                        // レンダラー追加（SimplePlaneRendererComponentは3D用）
                        Color shadowColor = Color(0.0f, 0.0f, 0.0f, 0.5f);
                        newObject->AddMeshComponent<SimplePlaneRendererComponent>(shadowColor, m_ShadowTexturePath);
                    }

                    GameObject* shadowObj = shadowObjects[shadowCnt];

                    // 表示
                    shadowObj->SetActive(true);

                    // 元オブジェクトの描画位置
                    float baseX = m_DrawStartPosX + x * m_SizePiece;
                    float baseZ = m_DrawStartPosZ + z * m_SizePiece;

                    float sizeShadow = m_SizePiece * 0.5f;

                    // スケール計算（伸びる方向にLength分拡大）
                    float scaleX = sizeShadow;
                    float scaleZ = sizeShadow;

                    if (shadowDirX != 0)
                    {
                        scaleX = sizeShadow * param.length;
                    }
                    if (shadowDirZ != 0)
                    {
                        scaleZ = sizeShadow * param.length;
                    }

                    // 位置計算（影の中心を伸びる方向にオフセット）
                    float offsetX = shadowDirX * (param.length * sizeShadow / 2.0f);
                    float offsetZ = shadowDirZ * (param.length * sizeShadow / 2.0f);

                    float posX = baseX + offsetX;
                    float posZ = baseZ + offsetZ;

                    // Transform更新
                    shadowObj->GetTransform().SetPosition(Vector3(posX, m_ShadowHeight, posZ));
                    shadowObj->GetTransform().SetScale(Vector3(scaleX, 1.0f, scaleZ));

                    shadowCnt++;
                }
            }
        }
    }
};
