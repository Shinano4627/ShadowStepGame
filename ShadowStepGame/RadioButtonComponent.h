// ===================================================================
// RadioButtonComponent.h
// ラジオボタンコンポーネント
// 複数のButtonComponentを管理し、排他的な選択状態を制御する
// ===================================================================
#pragma once
#include "Component.h"
#include "GameObject.h"
#include "ButtonComponent.h"
#include <vector>

// ===================================================================
// RadioButtonComponent クラス
// ===================================================================
class RadioButtonComponent : public Component
{
private:
    // ===================================================================
    // メンバ変数
    // ===================================================================
    std::vector<GameObject*> m_Buttons;     // 管理するボタンオブジェクト
    int m_SelectedIndex = -1;               // 現在選択中のインデックス（-1は未選択）

public:
    // ===================================================================
    // コンストラクタ・デストラクタ
    // ===================================================================
    RadioButtonComponent(const std::vector<GameObject*>& buttons)
        : m_Buttons(buttons)
        , m_SelectedIndex(-1)
    {
    }
    ~RadioButtonComponent() override = default;

    // ===================================================================
    // ライフサイクル
    // ===================================================================
    void Init() override;
    void Update() override;
    void Uninit() override;

    // ===================================================================
    // 選択状態
    // ===================================================================
    // 現在選択されているインデックスを取得（-1は未選択）
    int GetSelectedIndex() const { return m_SelectedIndex; }

    // 現在選択されているボタンオブジェクトを取得（nullptrは未選択）
    GameObject* GetSelectedButton() const;

    // インデックスで選択状態を設定
    void SetSelectedIndex(int index);

    // ボタン数を取得
    int GetButtonCount() const { return static_cast<int>(m_Buttons.size()); }

private:
    // ===================================================================
    // 内部処理
    // ===================================================================
    // 指定インデックス以外を非選択にする
    void DeselectOthers(int selectedIndex);
};
