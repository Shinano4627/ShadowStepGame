#pragma once
#include "Component.h"
#include "UnitCommon.h"
#include <vector>
// ===================================================================
// ユニットコンポネント
// ユニット一体分の状態・ターン管理
// ===================================================================

class UnitComponent :public Component
{
public:
    
private:
    // ユニット情報
    UnitStatus m_status;
    // アクション情報
    UnitAction m_action;

    int downTurn = 0;

    // -------- 各Flug --------
    bool m_isMyTurn = false;        // 自分のターンか
    bool m_actionConfirmed = false; // 行動確定済みか
    bool m_isActing = false;        // 行動中か
    bool m_turnFinished = false;    // ターン終了済みか

public:
 
    //=======================================
    // コンストラクタ・デストラクタ
    //=======================================
    UnitComponent(){}
    ~UnitComponent(){}

    //=======================================
    // ライフサイクル
    //=======================================
    void Init() override {};
    void Update() override;
    void Uninit() override {};

    //=======================================
    // 実行関数
    //=======================================
    // -------- ターン制御 --------
    void StartTurn();
    void EndTurn();

    // -------- 行動受付 --------
    void SetAction(const UnitAction& action);
    bool IsActionConfired() const
    {
        return m_actionConfirmed;
    }

    // -------- 実行 --------
    void ExcuteAction();

    // -------- 状態取得 --------
    bool IsTurnDinished() const
    {
        return m_turnFinished;
    }
    int GetSpeed() const
    {
        return m_status.speed;
    }
    bool IsDown() const
    {
        return m_status.isDown;
    }
    UnitType GetType() const
    {
        return m_status.type;
    }
    int GetId() const
    {
        return m_status.id;
    }
    MapPosition GetPosition() const
    {
        return m_status.pos;
    }
    UnitModel GetModel() const
    {
        return m_status.model;
    }

    void SetStatus(UnitStatus status) { m_status = status; }

    void SetDown(int turn)
    {
        m_status.isDown = true;
        downTurn = turn;

        std::cout << "Down!\n";
    }

    void RecoverDown()
    {
        downTurn--;
        if (downTurn <= 0)
        {
            m_status.isDown = false;
            downTurn = 0;
        }
    }

private:
    void Move();
    void Attack();
    void Place();

};
