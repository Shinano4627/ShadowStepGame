#pragma once
#include "Component.h"
#include "UnitCommon.h"
#include "UnitStateComponent.h"
#include <vector>

// ===================================================================
// ユニットコンポネント
// ユニット一体分の状態・ターン管理
// ===================================================================

// 前方宣言
class MeshRendererComponent;

class UnitComponent :public Component
{
public:

private:
    // ユニット情報
    UnitStatus m_status;
    // アクション情報
    UnitAction m_action;

    // ステータスアニメーション管理
    UnitStateComponent* m_unitStateComponent = nullptr;

    int downTurn = 0;

    // -------- 各Flug --------
    bool m_isMyTurn = false;        // 自分のターンか
    bool m_actionConfirmed = false; // 行動確定済みか
    bool m_isActing = false;        // 行動中か
    bool m_turnFinished = false;    // ターン終了済みか

    // 設定データ
    float m_WalkSpeed = 0.1f;
    float m_RotateSpeed = 0.1f;  // 回転速度（ラジアン/フレーム）

public:

    //=======================================
    // コンストラクタ・デストラクタ
    //=======================================
    UnitComponent(UnitStateComponent* unitStateComponent)
        :m_unitStateComponent(unitStateComponent) {
    }
    ~UnitComponent() {}

    //=======================================
    // ライフサイクル
    //=======================================
    void Init() override;
    void Update() override;
    void Uninit() override {};

    //=======================================
    // 実行関数
    //=======================================
    // -------- 行動制御 --------
    void Move();
    void Attack();
    void Place();

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

    // -------- KILLされた --------
    void Killed();

    // -------- 状態取得 --------
    bool IsTurnFinished() const
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
    bool IsAnimationFinished()
    {
        return m_unitStateComponent->GetState() == UnitState::UnitState::Down
            && !m_pOwner->GetMeshComponent<MeshRendererComponent>()->GetDoAnimation();
    }

    void SetStatus(UnitStatus status) { m_status = status; }

    void SetDown(int turn)
    {
        m_status.isDown = true;
        downTurn = turn;

        // ステータス変更
        m_unitStateComponent->ChangeState(UnitState::UnitState::Damaged);

        std::cout << "Down!\n";
    }

    void RecoverDown()
    {
        downTurn--;
        if (downTurn <= 0)
        {
            // ステータス変更
            m_unitStateComponent->ChangeState(UnitState::UnitState::Idle);

            m_status.isDown = false;
            downTurn = 0;
        }
    }

    bool IsActing() { return m_isActing; }

public:
    void RotateForTarget(const DirectX::SimpleMath::Vector3& target);  // ターゲットの方をむく。最終的にはGameObjectに移動させたい
};
