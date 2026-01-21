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
    float distance = 0;            // 移動に使用する幅（ターン数にも使用）
    int startX = 0;
    int startZ = 0; 
    int axis = 0;               // 0: X軸移動, 1: Z軸移動
    int direction = 1;          // 1: 正方向, -1: 負方向
    float otherAxisPos = 0;     // 固定軸の座標
    DirectX::SimpleMath::Color color;
};

class SunManageComponent : public Component
{
private:
    std::vector<SunData> m_SunList;
    int m_CurIdx = 0;
    int m_CurPosX = 0;          // マップ升目上のX座標（左上が0）
    int m_CurPosZ = 0;          // マップ升目上のZ座標（左上が0）
    int m_MapSizeWidth = 0;
    int m_MapSizeHeight = 0;
    int m_MapWidth = 0;
    int m_MapHeight = 0;
    int m_TurnProgress = 0;     // 経過ターン数
    float m_Offset = 10.f;      // オフセット値（仮）
public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SunManageComponent(float widthMapSize, float heightMapSize, int widthMap, int heightMap)
        : m_MapSizeWidth(widthMapSize)
        , m_MapSizeHeight(heightMapSize)
        , m_MapWidth(widthMap)
        , m_MapHeight(heightMap)
    {
        float CenterMapX = 0;
        float CenterMapZ = 0;

        // -z → z（南から北へ移動）
        // heightターンかけて -(height+offset) から (height+offset) へ移動
        {
            SunData newdata;
            newdata.distance = heightMapSize + m_Offset * 2;
            newdata.axis = 1;           // Z軸移動
            newdata.direction = 1;      // 正方向（-z → z）
            newdata.otherAxisPos = CenterMapX;
            newdata.color = DirectX::SimpleMath::Color(1.f, 1.f, 0.f, 1.f);
            newdata.startX = m_MapWidth / 2;
            newdata.startZ = 0;
            m_SunList.push_back(newdata);
        }

        // z → -z（北から南へ移動）
        // heightターンかけて (height+offset) から -(height+offset) へ移動
        {
            SunData newdata;
            newdata.distance = heightMapSize + m_Offset * 2;
            newdata.axis = 1;           // Z軸移動
            newdata.direction = -1;     // 負方向（z → -z）
            newdata.otherAxisPos = CenterMapX;
            newdata.color = DirectX::SimpleMath::Color(1.f, 0.f, 1.f, 1.f);
            newdata.startX = m_MapWidth / 2;
            newdata.startZ = m_MapHeight - 1;
            m_SunList.push_back(newdata);
        }

        // -x → x（西から東へ移動）
        // widthターンかけて -(width+offset) から (width+offset) へ移動
        {
            SunData newdata;
            newdata.distance = widthMapSize + m_Offset * 2;
            newdata.axis = 0;           // X軸移動
            newdata.direction = 1;      // 正方向（-x → x）
            newdata.otherAxisPos = CenterMapZ;
            newdata.color = DirectX::SimpleMath::Color(0.f, 1.f, 1.f, 1.f);
            newdata.startX = 0;
            newdata.startZ = m_MapHeight/2;
            m_SunList.push_back(newdata);
        }

        // x → -x（東から西へ移動）
        // widthターンかけて (width+offset) から -(width+offset) へ移動
        {
            SunData newdata;
            newdata.distance = widthMapSize + m_Offset * 2;
            newdata.axis = 0;           // X軸移動
            newdata.direction = -1;     // 負方向（x → -x）
            newdata.otherAxisPos = CenterMapZ;
            newdata.color = DirectX::SimpleMath::Color(0.f, 1.f, 0.f, 1.f);
            newdata.startX = m_MapWidth -1;
            newdata.startZ = m_MapHeight / 2;
            m_SunList.push_back(newdata);
        }
    }

    // ===================================================================
    // 初期化処理
    // ===================================================================
    void Init() override
    {
        // 初期位置を設定
        SetSunPosition(0);
    }

    // ===================================================================
    // 太陽の位置を計算・設定
    // ===================================================================
    void SetSunPosition(int turnProgress)
    {
        if (!m_pOwner) return;

        const SunData& data = m_SunList[m_CurIdx];
        float distance = data.distance;

        // 1ターンあたりの移動量
        // direction * distance + offset / MapWidth
        float movePerTurn;
        if (data.axis == 0)
        {
            // X軸移動

            movePerTurn = (data.direction * distance) / m_MapWidth;
        }
        else
        {
            // Z軸移動
            movePerTurn = (data.direction * distance) / m_MapHeight;
        }

        // 開始位置
        float startPos = data.direction * distance / 2;
        if (data.direction == 1)
        {
            startPos = -1 * distance / 2;
        }
        else
        {
            startPos = distance / 2;
        }
        

        // 現在位置
        float axisPos = startPos + movePerTurn * turnProgress;

        // 高さを計算: y = Width * (1 - x² / Width²)
        float clampedPos = axisPos;
        if (clampedPos < -distance) clampedPos = -distance;
        if (clampedPos > distance) clampedPos = distance;

        float heightY = distance/2 * (1.f - (clampedPos * clampedPos) / (distance/2 * distance/2));
        if (heightY < 0.f) heightY = 0.f;

        // 位置を設定
        DirectX::SimpleMath::Vector3 newPos;
        if (data.axis == 0)
        {
            // X軸移動
            newPos.x = axisPos;
            newPos.y = heightY;
            newPos.z = data.otherAxisPos;

            // マップ升目座標を更新
            UpdateMapPosition(data.direction, 0);
        }
        else
        {
            // Z軸移動
            newPos.x = data.otherAxisPos;
            newPos.y = heightY;
            newPos.z = axisPos;

            // マップ升目座標を更新
            UpdateMapPosition(0, data.direction);
        }

        m_pOwner->GetTransform().SetPosition(newPos);
    }

    // ===================================================================
    // マップ升目座標を更新
    // ===================================================================
    void UpdateMapPosition(int addX, int addZ)
    {
        m_CurPosX += addX;
        m_CurPosZ += addZ;

        // マップ範囲内にクランプ
        if (m_CurPosX < 0) m_CurPosX = 0;
        if (m_CurPosX >= m_MapWidth) m_CurPosX = m_MapWidth - 1;
        if (m_CurPosZ < 0) m_CurPosZ = 0;
        if (m_CurPosZ >= m_MapHeight) m_CurPosZ = m_MapHeight - 1;

        std::cout << "Sun Posision X: " << m_CurPosX << " Z: " << m_CurPosZ << std::endl;
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

        const SunData& data = m_SunList[m_CurIdx];

        if (data.axis == 0)   // X軸
        {
            // ターン数が端に達したら次の太陽パターンへ
            if (m_TurnProgress >= m_MapWidth)
            {
                m_CurIdx = (m_CurIdx + 1) % static_cast<int>(m_SunList.size());
                m_TurnProgress = 0;
            }

            m_CurPosX = m_SunList[m_CurIdx].startX;
            m_CurPosZ = m_SunList[m_CurIdx].startZ;
        }
        else if (data.axis == 1)   // Z軸
        {
            // ターン数が端に達したら次の太陽パターンへ
            if (m_TurnProgress >= m_MapHeight)
            {
                m_CurIdx = (m_CurIdx + 1) % static_cast<int>(m_SunList.size());
                m_TurnProgress = 0;
            }

            m_CurPosX = m_SunList[m_CurIdx].startX;
            m_CurPosZ = m_SunList[m_CurIdx].startZ;
        }
        SetSunPosition(m_TurnProgress);
    }

    void AdvanceTurn()
    {
        m_TurnProgress++;

        const SunData& data = m_SunList[m_CurIdx];
        int maxProgress = (data.axis == 0) ? m_MapWidth : m_MapHeight;

        // パターン切り替え
        if (m_TurnProgress >= maxProgress)
        {
            m_CurIdx = (m_CurIdx + 1) % static_cast<int>(m_SunList.size());
            m_TurnProgress = 0;
        }

        SetSunPosition(m_TurnProgress);
    }

    // ===================================================================
    // 現在の太陽データを取得
    // ===================================================================
    const SunData& GetCurrentSunData() const
    {
        return m_SunList[m_CurIdx];
    }

    // ===================================================================
    // 現在のターン経過を取得
    // ===================================================================
    int GetTurnProgress() const
    {
        return m_TurnProgress;
    }

    // ===================================================================
    // 現在のマップ升目座標を取得
    // ===================================================================
    int GetCurPosX() const { return m_CurPosX; }
    int GetCurPosZ() const { return m_CurPosZ; }
};
