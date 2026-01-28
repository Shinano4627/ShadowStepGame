#include "UnitComponent.h"

void UnitComponent::Update()
{
    if (!m_isActing) return;

    switch (m_action.type)
    {
    case UnitActionType::Move:
        Move();
        break;

    case UnitActionType::Attack:
        Attack();
        break;

    case UnitActionType::Place:
        Place();
        break;
    default:
        // 何もしない
        break;
    }
}

// ターン開始
void UnitComponent::StartTurn()
{
    m_isMyTurn = true;
    m_actionConfirmed = false;
    m_turnFinished = false;
    m_isActing = false;
    m_action = {};
}

// ターン終了
void UnitComponent::EndTurn()
{
    m_isMyTurn = false;
    m_turnFinished = true;
}

// 行動セット
void UnitComponent::SetAction(const UnitAction& action)
{
    if (!m_isMyTurn) return;

    m_action = action;
    m_actionConfirmed = true;   // 行動確定完了
}

// 行動実行
void UnitComponent::ExcuteAction()
{
    if (!m_isMyTurn) return;    // 私のターンか
    if (!m_actionConfirmed) return; // 行動確定してるか
    if (m_isActing) return;

    m_isActing = true;

    // 行動開始時の初期化だけ
    switch (m_action.type)
    {
    case UnitActionType::Move:
        // 移動開始準備（開始位置保存など）
        break;
    case UnitActionType::Attack:
        // 攻撃モーション開始
        break;
    case UnitActionType::Place:
        // 設置開始
        break;
    }
}

//=======================================
// 各アクション
//=======================================
void UnitComponent::Move()
{
    if (true)
    {
        m_status.pos = m_action.targetGrid;
        m_isActing = false;
        m_turnFinished = true; // ← GameSystem が確定するなら不要
    }
}

void UnitComponent::Attack()
{
    // 攻撃アニメ・SEのトリガー用
    // 実際のダメージや死亡判定は
    // MapSystem / ShadowSystem 更新後に処理される想定
    if (true)
    {
        m_isActing = false;
        m_turnFinished = true; // ← GameSystem が確定するなら不要
    }
}

void UnitComponent::Place()
{
    // 設置そのものは MapSystem が行う
    // ここでは「設置行動をした」という事実のみ
    if (true)
    {
        m_isActing = false;
        m_turnFinished = true; // ← GameSystem が確定するなら不要
    }
}