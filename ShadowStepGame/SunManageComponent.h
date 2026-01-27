// ===================================================================
// SunManageComponent.h
// ステージ上の太陽の動きを制御するコンポーネント
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "IOManager.h"
#include "Game.h"
#include "UnitCommon.h"
#include "SimpleCubeRendererComponent.h"

// 太陽の種類と移動方向
enum SunType
{
    SunSouth = 0,       // 南からのぼる太陽
    SunNorth,       // 北から
    SunWest,        // 西から
    SunEast,        // 東から
};

struct SunData
{
    SunType type;
    MapPosition lightDirection = {0,0};  // 基本方向(のぼるときの方向）  高度0のときは距離が２倍、真上は0
    float distance = 0;            // 移動に使用する幅（ターン数にも使用）
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
    float m_MapSizeWidth = 0;
    float m_MapSizeHeight = 0;

    // 光の方向
    MapPosition m_Direction = MapPosition(0, 0);

    // ターン管理
    int m_TurnProgress = 0;     // 経過ターン数
    int m_MaxTurn = 0;

    // 表示用
    float m_Offset = 10.f;      // オフセット値（仮）
private:
    // ===================================================================
// 太陽の位置を計算・設定
// ===================================================================
    void UpdateSunPosition(int turnProgress)
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

            movePerTurn = (data.direction * distance) / m_MaxTurn;
        }
        else
        {
            // Z軸移動
            movePerTurn = (data.direction * distance) / m_MaxTurn;
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

        float heightY = distance / 2 * (1.f - (clampedPos * clampedPos) / (distance / 2 * distance / 2));
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
    void UpdateLightDirection(int turnProgress)
    {
        if (!m_pOwner) return;

        const SunData& data = m_SunList[m_CurIdx];

        // 真上が存在する場合、真上のときは距離なし
        if (m_MaxTurn % 2 == 0 && turnProgress - m_MaxTurn / 2)
        {
            m_Direction = { 0, 0 };
        }
        // 高度0のとき距離が２倍
        else if (turnProgress == 0 || turnProgress == m_MaxTurn)
        {
            m_Direction = data.lightDirection * 2;
        }
        // 半分以上ターン経過で向きを反転
        else if (turnProgress > (m_MaxTurn / 2))
        {
            m_Direction = data.lightDirection *  (- 1);
        }
        else
        {
            m_Direction = data.lightDirection;
        }
    }

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    SunManageComponent(float widthMapSize, float heightMapSize, int maxTurn = 9)
        : m_MapSizeWidth(widthMapSize)
        , m_MapSizeHeight(heightMapSize)
        , m_MaxTurn(maxTurn)
    {
        float CenterMapX = 0;
        float CenterMapZ = 0;

        // -z → z（南から北へ移動）
        // heightターンかけて -(height+offset) から (height+offset) へ移動
        {
            SunData newdata;
            newdata.type = SunSouth;
            newdata.lightDirection = { 0, -1 };
            newdata.distance = heightMapSize + m_Offset * 2;
            newdata.axis = 1;           // Z軸移動
            newdata.direction = 1;      // 正方向（-z → z）
            newdata.otherAxisPos = CenterMapX;
            newdata.color = DirectX::SimpleMath::Color(1.f, 1.f, 0.f, 1.f); // オレンジ
            m_SunList.push_back(newdata);
        }

        // z → -z（北から南へ移動）
        // heightターンかけて (height+offset) から -(height+offset) へ移動
        {
            SunData newdata;
            newdata.type = SunNorth;
            newdata.lightDirection = { 0, 1 };
            newdata.distance = heightMapSize + m_Offset * 2;
            newdata.axis = 1;           // Z軸移動
            newdata.direction = -1;     // 負方向（z → -z）
            newdata.otherAxisPos = CenterMapX;
            newdata.color = DirectX::SimpleMath::Color(1.f, 0.f, 1.f, 1.f); // 紫
            m_SunList.push_back(newdata);
        }

        // -x → x（西から東へ移動）
        // widthターンかけて -(width+offset) から (width+offset) へ移動
        {
            SunData newdata;
            newdata.type = SunWest;
            newdata.lightDirection = { -1, 0 };
            newdata.distance = widthMapSize + m_Offset * 2;
            newdata.axis = 0;           // X軸移動
            newdata.direction = 1;      // 正方向（-x → x）
            newdata.otherAxisPos = CenterMapZ;
            newdata.color = DirectX::SimpleMath::Color(0.f, 1.f, 1.f, 1.f); // 黄色
            m_SunList.push_back(newdata);
        }

        // x → -x（東から西へ移動）
        // widthターンかけて (width+offset) から -(width+offset) へ移動
        {
            SunData newdata;
            newdata.type = SunEast;
            newdata.lightDirection = { 1, 0 };
            newdata.distance = widthMapSize + m_Offset * 2;
            newdata.axis = 0;           // X軸移動
            newdata.direction = -1;     // 負方向（x → -x）
            newdata.otherAxisPos = CenterMapZ;
            newdata.color = DirectX::SimpleMath::Color(0.f, 1.f, 0.f, 1.f); // 緑
            m_SunList.push_back(newdata);
        }
    }

    // ===================================================================
    // 初期化処理
    // ===================================================================
    void Init() override
    {
        m_CurIdx = 0;

        // 初期位置を設定
        UpdateSunPosition(m_CurIdx);
        UpdateLightDirection(m_CurIdx);
        m_pOwner->GetMeshComponent<SimpleCubeRendererComponent>()->SetColor(m_SunList[m_CurIdx].color);
    }

    // ===================================================================
    // 更新処理
    // ===================================================================
    void Update() override
    {
        if (!m_pOwner) return;

#ifdef DEBUG
        if(IO_MANAGER.GetKeyDownKeyBord(VK_RETURN)) // エンターキーで進行
        {
            AdvanceTurn();
        }        
#endif // DEBUG

    }

    void AdvanceTurn()
    {
        m_TurnProgress++;

        const SunData& data = m_SunList[m_CurIdx];

        if (data.axis == 0)   // X軸
        {
            // ターン数が端に達したら次の太陽パターンへ
            if (m_TurnProgress >= m_MaxTurn)
            {
                m_CurIdx = (m_CurIdx + 1) % static_cast<int>(m_SunList.size());
                m_TurnProgress = 0;
            }
        }
        else if (data.axis == 1)   // Z軸
        {
            // ターン数が端に達したら次の太陽パターンへ
            if (m_TurnProgress >= m_MaxTurn)
            {
                m_CurIdx = (m_CurIdx + 1) % static_cast<int>(m_SunList.size());
                m_TurnProgress = 0;
            }
        }
        UpdateSunPosition(m_TurnProgress);
        UpdateLightDirection(m_TurnProgress);
        m_pOwner->GetMeshComponent<SimpleCubeRendererComponent>()->SetColor(m_SunList[m_CurIdx].color);
    }

    // ===================================================================
    // データ取得
    // ===================================================================
    const SunData& GetCurrentSunData() const
    {
        return m_SunList[m_CurIdx];     // 現在の太陽データを取得
    }
    int GetTurnProgress() const
    {
        return m_TurnProgress;      // 現在のターン経過を取得
    }
    MapPosition GetDirection() const
    {
        return m_Direction;      // 現在の方向を取得
    }
};