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
#include "SimplePlaneRendererComponent.h"
#include <algorithm>
#include <vector>

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

    int m_MapWidth = 0;
    int m_MapHeight = 0;

    // 描画用パラメータ
    float m_SizePiece = 0.0f;
    float m_DrawStartPosX = 0.0f;
    float m_DrawStartPosZ = 0.0f;

    // 影オブジェクトの高さ（マップタイルより少し上）
    const float m_ShadowHeight = 0.3f;

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
    void SetUp(int mapWidth, int mapHeight, float sizePiece, float startPosX, float startPosZ, GameObjectList* gameObjectList)
    {
        m_MapWidth = mapWidth;
        m_MapHeight = mapHeight;
        m_SizePiece = sizePiece;
        m_DrawStartPosX = startPosX;
        m_DrawStartPosZ = startPosZ;

        MakeShadowMap();
        CreateShadowObjects(gameObjectList);
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
        // const std::vector<CellPosision>& unitPositions
    )
    {
        //=======================================
        // 初期化
        //=======================================
        ClearShadowMap();

        //=======================================
        // 【地形影】
        // 壁・樹などの不変オブジェクト
        //=======================================
        for (int z = 0; z < m_MapHeight; ++z)
        {
            for (int x = 0; x < m_MapWidth; ++x)
            {
                // 影を落とすオブジェクトのみ
                if (mapData[z][x] != 1 && mapData[z][x] != 4)
                    continue;

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
                    }
                }
            }
        }

        //=======================================
        // 影オブジェクトの表示/非表示を更新
        //=======================================
        UpdateShadowObjects(gameObjectList);
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

private:
    //=======================================
    // 影マップ生成
    //=======================================
    void MakeShadowMap()
    {
        m_ShadowMapData = new int* [m_MapHeight];
        for (int z = 0; z < m_MapHeight; ++z)
        {
            m_ShadowMapData[z] = new int[m_MapWidth];
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
            }
        }
    }

    //=======================================
    // 影オブジェクト生成（最大数を確保）
    //=======================================
    void CreateShadowObjects(GameObjectList* gameObjectList)
    {
        if (!gameObjectList) return;

        int cnt = 0;

        // ShadowTemplateを取得
        GameObject* shadowTemplate = gameObjectList->FindGameObjectWithTag("ShadowTemplate");
        if (!shadowTemplate)
        {
            std::cout << "[ShadowSystem] ShadowTemplateが見つかりません！" << std::endl;
            return;
        }

        // テンプレートからテクスチャパスを取得
        std::string texturePath = "";
        auto* templateRenderer = shadowTemplate->GetMeshComponent<SimplePlaneRendererComponent>();
        if (templateRenderer)
        {
            texturePath = templateRenderer->GetTexturePath();
        }

        // マップサイズ分の影オブジェクトを生成
        int objectId = 5000; // 影オブジェクト用ID開始値
        for (int z = 0; z < m_MapHeight; ++z)
        {
            for (int x = 0; x < m_MapWidth; ++x)
            {
                // 位置計算
                float posX = m_DrawStartPosX + x * m_SizePiece;
                float posZ = m_DrawStartPosZ + z * m_SizePiece;

                // オブジェクト生成
                auto obj = std::make_unique<GameObject>(
                    Vector3(posX, m_ShadowHeight, posZ),
                    Vector3::Zero,
                    Vector3(m_SizePiece / 2, 1.0f, m_SizePiece / 2)
                );
                GameObject* newObject = obj.get();
                newObject->SetID(objectId++);
                newObject->SetName("Shadow");
                newObject->SetTag("Shadow");

                // レンダラー追加
                Color shadowColor = Color(0.0f, 0.0f, 0.0f, 0.5f);
                newObject->AddMeshComponent<SimplePlaneRendererComponent>(shadowColor, texturePath);

                // 初期状態は非表示
                newObject->SetActive(false);

                gameObjectList->AddObject(std::move(obj));
                cnt++;
            }
        }

        std::cout << "[ShadowSystem] 影オブジェクトを" << cnt << "個生成しました" << std::endl;
    }

    //=======================================
    // 影オブジェクトの表示/非表示を更新
    //=======================================
    void UpdateShadowObjects(GameObjectList* gameObjectList)
    {
        for (int z = 0; z < m_MapHeight; ++z)
        {
            for (int x = 0; x < m_MapWidth; ++x)
            {
                std::vector<GameObject*> shadowObjects = gameObjectList->FindGameObjectsWithTag("Shadow");
                int index = z * m_MapWidth + x;
                if (index >= static_cast<int>(shadowObjects.size())) continue;

                GameObject* shadowObj = shadowObjects[index];
                if (!shadowObj) continue;

                // 影マップに影があれば表示、なければ非表示
                bool hasShadow = (m_ShadowMapData[z][x] == 1);
                shadowObj->SetActive(hasShadow);

                // 位置を更新（必要に応じて）
                if (hasShadow)
                {
                    float posX = m_DrawStartPosX + x * m_SizePiece;
                    float posZ = m_DrawStartPosZ + z * m_SizePiece;
                    shadowObj->GetTransform().SetPosition(Vector3(posX, m_ShadowHeight, posZ));
                }
            }
        }
    }
};
