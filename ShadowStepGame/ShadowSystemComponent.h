//=======================================
// ShadowSystemComponent.h
// すべての影を管理
//=======================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include <algorithm>

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

public:
    //=======================================
    // コンストラクタ
    //=======================================
    ShadowSystemComponent() {}

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
    void SetUp(int mapWidth, int mapHeight)
    {
        m_MapWidth = mapWidth;
        m_MapHeight = mapHeight;
        MakeShadowMap();
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
        const ShadowParam& param
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
    // 【将来拡張】
    // Unit の影（同方向・同長さ）
    //=======================================
    /*
    for (const auto& unitPos : unitPositions)
    {
        int shadowX = unitPos.column;
        int shadowZ = unitPos.row;

        for (int i = 0; i < param.length; ++i)
        {
            shadowX += param.dirX;
            shadowZ += param.dirZ;

            if (shadowX < 0 || shadowX >= m_MapWidth ||
                shadowZ < 0 || shadowZ >= m_MapHeight)
                break;

            if (mapData[shadowZ][shadowX] == 0)
            {
                m_ShadowMapData[shadowZ][shadowX] = 1;
            }
        }
    }
    */
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
};
