#include "UnitComponent.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"

void UnitComponent::Init()
{
}

void UnitComponent::Update()
{
    if (!m_isActing) return;
}

// ターン開始
void UnitComponent::StartTurn()
{
    m_isMyTurn = true;
    m_actionConfirmed = false;
    m_turnFinished = false;
    m_isActing = true;
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

    // ステータス変更
    UnitState::UnitState newState = UnitState::UnitState::Idle;
    switch (action.type)
    {
    case 	UnitActionType::Move:
        newState = UnitState::UnitState::Move;
        break;
    case UnitActionType::Attack:
        newState = UnitState::UnitState::Attack;
        break;
    case UnitActionType::Place:
        newState = UnitState::UnitState::Place;
        break;
    case UnitActionType::ShadowMove:
        newState = UnitState::UnitState::ShadowMove;
        break;
    default:
        break;
    }
    m_unitStateComponent->ChangeState(newState);

    m_action = action;
    m_actionConfirmed = true;   // 行動確定完了
}

// 行動実行
void UnitComponent::ExcuteAction()
{
    if (!m_isMyTurn) return;    // 私のターンか
    if (!m_actionConfirmed) return; // 行動確定してるか
    if (!m_isActing) return;

    // 行動開始時の初期化だけ
    Transform& transform = m_pOwner->GetTransform();
    switch (m_action.type)
    {
    case UnitActionType::Move:
        // 移動開始準備（開始位置保存など）
    {
        Vector3 currentPos = transform.GetPosition();
        Vector3 targetPos = transform.GetPosition();
        targetPos.x = m_action.targetGrid.x * 5.0f;
        targetPos.z = m_action.targetGrid.z * 5.0f;
        Vector3 newPos = DirectX::SimpleMath::Vector3::Lerp(currentPos, targetPos, m_WalkSpeed);
        transform.SetPosition(newPos);

        // 移動完了
        if ((targetPos - newPos).Length() <= 1e-1f)
        {
            // 位置あわせ
            transform.SetPosition(targetPos);
            m_unitStateComponent->ChangeState(UnitState::UnitState::Idle);
            m_turnFinished = true;
            m_isActing = false;
        }
    }
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
        std::cout << m_pOwner->GetName() << " : Move to Map Position x : " << m_action.targetGrid.x << " z : " << m_action.targetGrid.z << std::endl;
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