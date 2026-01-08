// ===================================================================
// SunManageComponent.h
// ステージ上の太陽の動きを制御するコンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"

struct SunData
{
    float startPosX = 0;
    float startPosZ = 0;
    DirectX::SimpleMath::Vector2 moveDir;   // x,　z軸の移動　y軸は計算で算出する
    DirectX::SimpleMath::Color color;
};

class SunManageComponent : public Component
{
private:
    std::vector<SunData> m_SunList;
    int m_CurIdx = 0;
    int m_CurPosMapX = 0;
    int m_CurPosMapY = 0;
    int m_MapWidth = 0;
    int m_MapHeight = 0;
    int m_TurnProgress = 0;     // 一周するまでの経過時間
public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SunManageComponent(float widthMap, float heightMap)
        : m_MapWidth(widthMap)
        , m_MapHeight(heightMap)
    {
        float CenterMapX = 0;
        float CenterMapZ = 0;
        float offset = 35.f;

        // -z →　z
        {
            SunData newdata;
            newdata.startPosX = CenterMapX;
            newdata.startPosZ = -heightMap;
            newdata.moveDir = DirectX::SimpleMath::Vector2(0.f, (heightMap + offset - 2.5) / 7);
            newdata.color = DirectX::SimpleMath::Color(1.f, 1.f, 0.f, 1.f);
            m_SunList.push_back(newdata);
        }

        // z →　-z
        {
            SunData newdata;
            newdata.startPosX = CenterMapX;
            newdata.startPosZ = heightMap;
            newdata.moveDir = DirectX::SimpleMath::Vector2(0.f, -(heightMap + offset + 2.5) / 7);
            newdata.color = DirectX::SimpleMath::Color(1.f, 0.f, 1.f, 1.f);
            m_SunList.push_back(newdata);
        }

        // -x →　x
        {
            SunData newdata;
            newdata.startPosX = -widthMap;
            newdata.startPosZ = CenterMapZ;
            newdata.moveDir = DirectX::SimpleMath::Vector2((widthMap + offset - 2.5) / 7, 0.f);
            newdata.color = DirectX::SimpleMath::Color(0.f, 1.f, 1.f, 1.f);
            m_SunList.push_back(newdata);
        }
        // x →　-x
        {
            SunData newdata;
            newdata.startPosX = widthMap;
            newdata.startPosZ = CenterMapZ;
            newdata.moveDir = DirectX::SimpleMath::Vector2(-(widthMap + offset + 2.5) / 7, 0.f);
            newdata.color = DirectX::SimpleMath::Color(0.f, 1.f, 0.f, 1.f);
            m_SunList.push_back(newdata);
        }
    }

    // ===================================================================
    // 初期化処理
    // ===================================================================
    void Init() override
    {
        m_pOwner->GetTransform().SetPosition(DirectX::SimpleMath::Vector3(m_SunList[0].startPosX, 0.f, m_SunList[0].startPosZ));
        m_CurPosMapX = m_SunList[0].startPosX;
        m_CurPosMapY = m_SunList[0].startPosZ;
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_pOwner) return;

        if (!IO_MANAGER.GetKeyDown(TYPE_OK))
        {
            return;
        }

        m_TurnProgress++;
        m_CurPosMapX++;
        m_CurPosMapY++;

        Transform& transform = m_pOwner->GetTransform();
        DirectX::SimpleMath::Vector3 newPos;
        newPos.x = m_SunList[m_CurIdx].startPosX;
        newPos.z = m_SunList[m_CurIdx].startPosZ;
        newPos.x += m_SunList[m_CurIdx].moveDir.x * m_TurnProgress;
        newPos.z += m_SunList[m_CurIdx].moveDir.y * m_TurnProgress;

        if (m_CurIdx == 0 || m_CurIdx == 1)
        {
            if (newPos.z * newPos.z <= 0.01)
            {
                newPos.y = m_MapHeight;
            }
            else
            {
                newPos.y = -(newPos.z * newPos.z) / 35 + 35;
            }            
        }
        else
        {
            if (newPos.x * newPos.x <= 0.01)
            {
                newPos.y = m_MapHeight;
            }
            else
            {
                newPos.y = -(newPos.x * newPos.x) / 35 + 35;
            }
        }

        if (m_TurnProgress >= 7)
        {
            m_CurIdx = (m_CurIdx + 1) % 4;
            newPos.x = m_SunList[m_CurIdx].startPosX;
            newPos.z = m_SunList[m_CurIdx].startPosZ;
            newPos.y = 0;


            m_TurnProgress = 0;
        }

        transform.SetPosition(newPos);
    }

    // ===================================================================
    // 設定
    // ===================================================================
};