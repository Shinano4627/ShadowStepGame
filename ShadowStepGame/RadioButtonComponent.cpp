// ===================================================================
// RadioButtonComponent.cpp
// ラジオボタンコンポーネント実装
// ===================================================================
#include "RadioButtonComponent.h"

// ===================================================================
// 初期化処理
// ===================================================================
void RadioButtonComponent::Init()
{
    // 一つ目を選択状態にする
    m_SelectedIndex = 0;
    m_Buttons[0]->GetComponent<ButtonComponent>()->SetSelected(true);
}

// ===================================================================
// 更新処理
// ===================================================================
void RadioButtonComponent::Update()
{
    // 各ボタンの選択状態をチェック
    for (int i = 0; i < static_cast<int>(m_Buttons.size()); i++)
    {
        GameObject* buttonObj = m_Buttons[i];
        if (!buttonObj) continue;

        ButtonComponent* button = buttonObj->GetComponent<ButtonComponent>();
        if (!button) continue;

        // 選択状態になったボタンを検出
        if (button->IsSelected())
        {
            // 現在の選択と異なる場合のみ処理
            if (m_SelectedIndex != i)
            {
                m_SelectedIndex = i;
                DeselectOthers(i);

                return; // 一つ見つかったら終了
            }
        }
    }
}

// ===================================================================
// 終了処理
// ===================================================================
void RadioButtonComponent::Uninit()
{
    m_Buttons.clear();
    m_SelectedIndex = -1;
}

// ===================================================================
// 現在選択されているボタンオブジェクトを取得
// ===================================================================
GameObject* RadioButtonComponent::GetSelectedButton() const
{
    if (m_SelectedIndex >= 0 && m_SelectedIndex < static_cast<int>(m_Buttons.size()))
    {
        return m_Buttons[m_SelectedIndex];
    }
    return nullptr;
}

// ===================================================================
// インデックスで選択状態を設定
// ===================================================================
void RadioButtonComponent::SetSelectedIndex(int index)
{
    if (index < 0 || index >= static_cast<int>(m_Buttons.size()))
    {
        // 無効なインデックスの場合、全て非選択に
        m_SelectedIndex = -1;
        for (auto* buttonObj : m_Buttons)
        {
            if (!buttonObj) continue;
            ButtonComponent* button = buttonObj->GetComponent<ButtonComponent>();
            if (button)
            {
                button->SetSelected(false);
            }
        }
        return;
    }

    m_SelectedIndex = index;

    // 指定インデックスを選択、他を非選択に
    for (int i = 0; i < static_cast<int>(m_Buttons.size()); i++)
    {
        GameObject* buttonObj = m_Buttons[i];
        if (!buttonObj) continue;

        ButtonComponent* button = buttonObj->GetComponent<ButtonComponent>();
        if (button)
        {
            button->SetSelected(i == index);
        }
    }
}

// ===================================================================
// 指定インデックス以外を非選択にする
// ===================================================================
void RadioButtonComponent::DeselectOthers(int selectedIndex)
{
    for (int i = 0; i < static_cast<int>(m_Buttons.size()); i++)
    {
        if (i == selectedIndex) continue;

        GameObject* buttonObj = m_Buttons[i];
        if (!buttonObj) continue;

        ButtonComponent* button = buttonObj->GetComponent<ButtonComponent>();
        if (button)
        {
            button->SetSelected(false);
        }
    }
}
