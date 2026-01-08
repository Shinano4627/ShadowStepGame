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
    float offset = 0;           // 地平線下のオフセット
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
    float m_MapSizeWidth = 0;   // マップの実際の大きさ
    float m_MapSizeHeight = 0;
    int m_MapWidth = 0;      // 升目上のマップの大きさ
    int m_MapHeight = 0;
    int m_TurnProgress = 0;     // 経過ターン数
    float m_Offset = 20.f;      // オフセット値（仮）
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
            newdata.distance = heightMapSize;
            newdata.offset = m_Offset;
            newdata.axis = 1;           // Z軸移動
            newdata.direction = 1;      // 正方向（-z → z）
            newdata.otherAxisPos = CenterMapX;
            newdata.color = DirectX::SimpleMath::Color(1.f, 1.f, 0.f, 1.f);
            m_SunList.push_back(newdata);
        }

        // z → -z（北から南へ移動）
        // heightターンかけて (height+offset) から -(height+offset) へ移動
        {
            SunData newdata;
            newdata.distance = heightMapSize;
            newdata.offset = m_Offset;
            newdata.axis = 1;           // Z軸移動
            newdata.direction = -1;     // 負方向（z → -z）
            newdata.otherAxisPos = CenterMapX;
            newdata.color = DirectX::SimpleMath::Color(1.f, 0.f, 1.f, 1.f);
            m_SunList.push_back(newdata);
        }

        // -x → x（西から東へ移動）
        // widthターンかけて -(width+offset) から (width+offset) へ移動
        {
            SunData newdata;
            newdata.distance = widthMapSize;
            newdata.offset = m_Offset;
            newdata.axis = 0;           // X軸移動
            newdata.direction = 1;      // 正方向（-x → x）
            newdata.otherAxisPos = CenterMapZ;
            newdata.color = DirectX::SimpleMath::Color(0.f, 1.f, 1.f, 1.f);
            m_SunList.push_back(newdata);
        }

        // x → -x（東から西へ移動）
        // widthターンかけて (width+offset) から -(width+offset) へ移動
        {
            SunData newdata;
            newdata.distance = widthMapSize;
            newdata.offset = m_Offset;
            newdata.axis = 0;           // X軸移動
            newdata.direction = -1;     // 負方向（x → -x）
            newdata.otherAxisPos = CenterMapZ;
            newdata.color = DirectX::SimpleMath::Color(0.f, 1.f, 0.f, 1.f);
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
        float width = data.distance;
        float offset = data.offset;
        int totalTurns = static_cast<int>(width);

        // 現在のターンでの移動軸座標を計算
        // 開始位置: -(width + offset)
        // 終了位置: (width + offset)
        // 1ターンあたりの移動量: (2 * width + 2 * offset) / width
        float totalDistance = 2.f * width + 2.f * offset;
        float movePerTurn = totalDistance / width;

        float axisPos;
        if (data.direction == 1)
        {
            // 正方向: -(width+offset) → (width+offset)
            axisPos = -(width + offset) + movePerTurn * turnProgress;
        }
        else
        {
            // 負方向: (width+offset) → -(width+offset)
            axisPos = (width + offset) - movePerTurn * turnProgress;
        }

        // 高さを計算: y = Width * (1 - x² / Width²)
        // ただしxは-Width〜Widthの範囲でクランプして計算
        float clampedPos = axisPos;
        if (clampedPos < -width) clampedPos = -width;
        if (clampedPos > width) clampedPos = width;

        float heightY = width * (1.f - (clampedPos * clampedPos) / (width * width));
        if (heightY < 0.f) heightY = 0.f;

        // 位置を設定
        DirectX::SimpleMath::Vector3 newPos;
        if (data.axis == 0)
        {
            // X軸移動
            newPos.x = axisPos;
            newPos.y = heightY;
            newPos.z = data.otherAxisPos;
        }
        else
        {
            // Z軸移動
            newPos.x = data.otherAxisPos;
            newPos.y = heightY;
            newPos.z = axisPos;
        }

        m_pOwner->GetTransform().SetPosition(newPos);
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
        int totalTurns = static_cast<int>(data.distance);

        // ターン数が width に達したら次の太陽パターンへ
        if (m_TurnProgress >= totalTurns)
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
};
