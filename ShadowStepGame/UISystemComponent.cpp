#include "UISystemComponent.h"
#include "GameObjectList.h"

// コンポーネント
#include "Texture2D.h"
#include "ButtonComponent.h"
#include "RadioButtonComponent.h"
#include "UIAnimationComponent.h"

// ===================================================================
// UIオブジェクトの追加・設定
// ===================================================================
void UISystemComponent::SetUIObject(std::unique_ptr<GameObjectList>& objectList)
{
    m_pUIObjects = objectList.get();

    MakeUIButtons(objectList);
    MakeUITimeLine(objectList);
    MakeUIStatus(objectList);
    MakeUISunRoute(objectList);

    // 一旦タイムライン用オブジェクトを非表示に
    std::vector<GameObject*> timelineIcons = objectList->FindGameObjectsWithTag("UITimeline");
    for (auto icon : timelineIcons)
    {
        icon->SetActive(false);
    }
        
}

void UISystemComponent::MakeUIButtons(std::unique_ptr<GameObjectList>& objectList)
{
    // UIAreaButtonsの範囲を取得
    GameObject* buttonArea = objectList->FindGameObjectWithName("UIAreaButtons");
    if (!buttonArea)
    {
        std::cout << "[SceneGame] UIAreaButtons not found!" << std::endl;
        return;
    }

    Vector3 areaPos = buttonArea->GetTransform().GetPosition();
    Vector3 areaScale = buttonArea->GetTransform().GetScale();

    const float padding = 5.f;
    const int rowCount = 3;
    float rowHeight = (areaScale.y - padding * (rowCount + 1)) / rowCount;
    float topY = areaPos.y + areaScale.y / 2.f;

    // ---------------------------------------------------------------
    // 1行目: ButtonSub（横並び）
    // ---------------------------------------------------------------
    std::vector<GameObject*> buttonSubs = objectList->FindGameObjectsWithTag("ButtonSub");
    int subCount = static_cast<int>(buttonSubs.size());
    if (subCount > 0)
    {
        float row1Y = topY - padding - rowHeight / 2.f;
        float subWidth = (areaScale.x - padding * (subCount + 1)) / subCount;
        float startX = areaPos.x - areaScale.x / 2.f + padding + subWidth / 2.f;

        for (int i = 0; i < subCount; i++)
        {
            float posX = startX + i * (subWidth + padding);
            buttonSubs[i]->GetTransform().SetPosition(Vector3(posX, row1Y, 0.f));
            buttonSubs[i]->GetTransform().SetScale(Vector3(subWidth, rowHeight, 1.f));

            auto* tex = buttonSubs[i]->GetMeshComponent<Texture2D>();
            tex->SetUV(1, 1, 2, 1);

            buttonSubs[i]->AddComponent<ButtonComponent>(buttonSubs[i]->GetTransform().GetScale());
            buttonSubs[i]->SetActive(false);
        }
    }

    // ラジオボタンコンポーネント（ButtonSub用）
    std::unique_ptr<GameObject> radioSubButton = std::make_unique<GameObject>();
    radioSubButton->SetName("RadioSubButton");
    radioSubButton->SetTag("RadioButton");
    radioSubButton->AddComponent<RadioButtonComponent>(buttonSubs);
    objectList->AddObject(std::move(radioSubButton));

    // ---------------------------------------------------------------
    // 2行目: 攻撃ボタン / 配置ボタン（同位置に重ねる）
    // ---------------------------------------------------------------
    GameObject* buttonAttack = nullptr;
    GameObject* buttonBuild = nullptr;
    GameObject* buttonMove = nullptr;

    std::vector<GameObject*> buttons = objectList->FindGameObjectsWithTag("Button");

    // ボタンをオブジェクトリストから取得 or 作成
    for (auto* btn : buttons)
    {
        const std::string& name = btn->GetName();
        if (name == "ButtonAttack") buttonAttack = btn;
        else if (name == "ButtonBuild") buttonBuild = btn;
        else if (name == "ButtonMove") buttonMove = btn;
    }

    if (!buttonAttack)
    {
        auto attackBtn = std::make_unique<GameObject>();
        attackBtn->SetName("ButtonAttack");
        attackBtn->SetTag("Button");
        objectList->AddObject(std::move(attackBtn));
        buttonAttack = objectList->FindGameObjectWithName("ButtonAttack");
    }

    if (!buttonBuild)
    {
        auto buildBtn = std::make_unique<GameObject>();
        buildBtn->SetName("ButtonBuild");
        buildBtn->SetTag("Button");
        objectList->AddObject(std::move(buildBtn));
        buttonBuild = objectList->FindGameObjectWithName("ButtonBuild");
    }

    if (!buttonMove)
    {
        auto moveBtn = std::make_unique<GameObject>();
        moveBtn->SetName("ButtonMove");
        moveBtn->SetTag("Button");
        objectList->AddObject(std::move(moveBtn));
        buttonMove = objectList->FindGameObjectWithName("ButtonMove");
    }

    // 2行目のY座標
    float row2Y = topY - padding - rowHeight / 2.f - 1 * (rowHeight + padding);
    // 3行目のY座標
    float row3Y = topY - padding - rowHeight / 2.f - 2 * (rowHeight + padding);
    float buttonWidth = areaScale.x - padding * 2;

    // ボタン共通設定
    auto setupButton = [&](GameObject* btn, ButtonComponent::UIButtonType type, float posY)
        {
            if (!btn) return;
            btn->GetTransform().SetPosition(Vector3(areaPos.x, posY, 0.f));
            btn->GetTransform().SetScale(Vector3(buttonWidth, rowHeight, 1.f));

            auto* tex = btn->GetMeshComponent<Texture2D>();
            if (tex) tex->SetUV(1, 1, 7, 1);

            if (!btn->GetComponent<ButtonComponent>())
            {
                auto* btnComp = btn->AddComponent<ButtonComponent>(btn->GetTransform().GetScale());
                btnComp->SetButtonType(type);
            }

            if (!btn->GetComponent<UIAnimationComponent>())
            {
                btn->AddComponent<UIAnimationComponent>();
            }
        };

    setupButton(buttonAttack, ButtonComponent::UIButtonType::Attack, row2Y);
    setupButton(buttonBuild, ButtonComponent::UIButtonType::Build, row2Y);
    setupButton(buttonMove, ButtonComponent::UIButtonType::Move, row3Y);

    // 初期状態は攻撃・配置非表示、移動表示
    if (buttonAttack) buttonAttack->SetActive(true);
    if (buttonBuild)  buttonBuild->SetActive(false);
    if (buttonMove)   buttonMove->SetActive(true);

    std::cout << "[SceneGame] Buttons arranged in UIAreaButtons" << std::endl;
}


//// ===================================================================
//// ボタン整列処理
//// ===================================================================
//void UISystemComponent::MakeUIButtons(std::unique_ptr<GameObjectList>& objectList)
//{
//    // UIAreaButtonsの範囲を取得
//    GameObject* buttonArea = objectList->FindGameObjectWithName("UIAreaButtons");
//
//    if (!buttonArea)
//    {
//        std::cout << "[SceneGame] UIAreaButtons not found!" << std::endl;
//        return;
//    }
//
//    Vector3 areaPos = buttonArea->GetTransform().GetPosition();
//    Vector3 areaScale = buttonArea->GetTransform().GetScale();
//
//    // パディング（仮値）
//    const float padding = 5.f;
//
//    // 行数: ButtonSub(1行) + Button(2行) = 3行
//    const int rowCount = 3;
//    float rowHeight = (areaScale.y - padding * (rowCount + 1)) / rowCount;
//
//    // 各行のY座標を計算（上から順に）
//    // エリアの上端からスタート
//    float topY = areaPos.y + areaScale.y / 2.f;
//
//    // ---------------------------------------------------------------
//    // 1行目: ButtonSub（横並び）
//    // ---------------------------------------------------------------
//    std::vector<GameObject*> buttonSubs = objectList->FindGameObjectsWithTag("ButtonSub");
//    int subCount = static_cast<int>(buttonSubs.size());
//
//    if (subCount > 0)
//    {
//        float row1Y = topY - padding - rowHeight / 2.f;
//        float subWidth = (areaScale.x - padding * (subCount + 1)) / subCount;
//        float startX = areaPos.x - areaScale.x / 2.f + padding + subWidth / 2.f;
//
//        for (int i = 0; i < subCount; i++)
//        {
//            float posX = startX + i * (subWidth + padding);
//            buttonSubs[i]->GetTransform().SetPosition(Vector3(posX, row1Y, 0.f));
//            buttonSubs[i]->GetTransform().SetScale(Vector3(subWidth, rowHeight, 1.f));
//            // UVを設定
//            auto* tex = buttonSubs[i]->GetMeshComponent<Texture2D>();
//            tex->SetUV(1, 1, 2, 1);
//            // ボタンコンポーネント追加
//            buttonSubs[i]->AddComponent<ButtonComponent>(buttonSubs[i]->GetTransform().GetScale());
//
//            //一旦非アクティブ
//            buttonSubs[i]->SetActive(false);
//        }
//    }
//
//    // ラジオボタンコンポーネントつきオブジェクト作成
//    std::unique_ptr<GameObject> radioSubButton = std::make_unique<GameObject>();
//    radioSubButton->SetName("RadioSubButton");
//    radioSubButton->SetTag("RadioButton");
//    radioSubButton->AddComponent<RadioButtonComponent>(buttonSubs);
//    objectList->AddObject(std::move(radioSubButton));
//
//    // ---------------------------------------------------------------
//    // 2行目・3行目: Button（ButtonBuild, ButtonMove）
//    // ---------------------------------------------------------------
//    // 表示するボタン名のリスト
//    std::vector<std::string> buttonNames = { "ButtonAttack","ButtonMove","ButtonBuild", };
//
//
//    // TagがButtonのオブジェクトから該当するものを取得
//    std::vector<GameObject*> buttons = objectList->FindGameObjectsWithTag("Button");
//    std::vector<GameObject*> targetButtons;
//
//    for (auto& name:buttonNames)
//    {
//        for (auto* btn : buttons)
//        {
//            if (std::string(btn->GetName()) == name)
//            {
//                targetButtons.push_back(btn);
//                break;
//            }
//        }
//    }
//
//
//    // ボタンを配置（2行目、3行目）
//    float buttonWidth = areaScale.x - padding * 2;
//    for (int i = 0; i < static_cast<int>(targetButtons.size()); i++)
//    {
//        // 行インデックス（1行目はButtonSubなので、i+1）
//        int rowIndex = i + 1;
//        float rowY = topY - padding - rowHeight / 2.f - rowIndex * (rowHeight + padding);
//        float posX = areaPos.x;
//
//        auto* btn = targetButtons[i];
//        btn->GetTransform().SetPosition(Vector3(posX, rowY, 0.f));
//        btn->GetTransform().SetScale(Vector3(buttonWidth, rowHeight, 1.f));
//
//        // UVを設定
//        auto* tex = btn->GetMeshComponent<Texture2D>();
//        tex->SetUV(1, 1, 3, 3);
//
//        //アニメーションコンポネント追加
//        btn->AddComponent<UIAnimationComponent>();
//        // ボタンコンポーネント追加
//        auto* btnComp=btn->AddComponent<ButtonComponent>(btn->GetTransform().GetScale());
//
//        if (btnComp)
//        {
//            if (std::string(btn->GetName()) == "ButtonBuild")
//            {
//                btnComp->SetButtonType(ButtonComponent::UIButtonType::Build);
//            }
//            else if(std::string(btn->GetName()) == "ButtonMove")
//            {
//                btnComp->SetButtonType(ButtonComponent::UIButtonType::Move);
//            }
//            else if (std::string(btn->GetName()) == "ButtonAttack")
//            {
//                btnComp->SetButtonType(ButtonComponent::UIButtonType::Attack);
//            }
//        }
//
//        if (std::string(btn->GetName()) == "ButtonMove")btn->SetActive(true);
//        else btn->SetActive(false);
//    }
//
//
//    // ラジオボタンコンポーネントつきオブジェクト作成
//    std::unique_ptr<GameObject> radioButton = std::make_unique<GameObject>();
//    int id = objectList->GetLastID();
//    radioButton->SetID(id);
//    radioButton->SetName("RadioButton");
//    radioButton->SetTag("RadioButton");
//    radioButton->AddComponent<RadioButtonComponent>(buttons);
//    objectList->AddObject(std::move(radioButton));
//
//    std::cout << "[SceneGame] Buttons arranged in UIAreaButtons" << std::endl;
//}

void UISystemComponent::MakeUITimeLine(std::unique_ptr<GameObjectList>& objectList)
{
    // UIAreaTimelineを名前で取得
    GameObject* timelineArea = objectList->FindGameObjectWithName("UIAreaTimeline");
    if (!timelineArea) return;

    // WindowTemplateからテクスチャパスを取得
    GameObject* windowTemplate = objectList->FindGameObjectWithTag("WindowTemplate");
    if (!windowTemplate) return;
    auto* templateTex = windowTemplate->GetMeshComponent<Texture2D>();
    if (!templateTex) return;

    // WindowTemplateを非表示にする
    windowTemplate->SetActive(false);

    // エリアの位置・スケールを取得
    Vector3 areaPos = timelineArea->GetTransform().GetPosition();
    Vector3 areaScale = timelineArea->GetTransform().GetScale();

    // タイムラインウィンドウを作成
    auto windowObj = std::make_unique<GameObject>(areaPos, Vector3(0, 0, 0), areaScale);
    int id = objectList->GetLastID();
    windowObj->SetID(id);
    windowObj->SetName("WindowTimeline");
    windowObj->SetTag("Window");
    windowObj->AddMeshComponent<Texture2D>(templateTex->GetTexturePath(), Color(1, 1, 1, 1));

    windowObj->SetActive(false);

    objectList->AddObject(std::move(windowObj));
    std::cout << "[SceneGame] Timeline window created" << std::endl;
}

void UISystemComponent::MakeUIStatus(std::unique_ptr<GameObjectList>& objectList)
{
    // UIAreaStatusを名前で取得
    GameObject* statusArea = objectList->FindGameObjectWithName("UIAreaStatus");
    if (!statusArea) return;

    // WindowTemplateからテクスチャパスを取得
    GameObject* windowTemplate = objectList->FindGameObjectWithTag("WindowTemplate");
    if (!windowTemplate) return;
    auto* templateTex = windowTemplate->GetMeshComponent<Texture2D>();
    if (!templateTex) return;

    // エリアの位置・スケールを取得
    Vector3 areaPos = statusArea->GetTransform().GetPosition();
    Vector3 areaScale = statusArea->GetTransform().GetScale();

    // ステータスウィンドウを作成
    auto windowObj = std::make_unique<GameObject>(areaPos, Vector3(0, 0, 0), areaScale);
    int id = objectList->GetLastID();
    windowObj->SetID(id);
    windowObj->SetName("WindowStatus");
    windowObj->SetTag("Window");
    windowObj->AddMeshComponent<Texture2D>(templateTex->GetTexturePath(), Color(1, 1, 1, 1));

    //一旦非アクティブ
    windowObj->SetActive(false);

    objectList->AddObject(std::move(windowObj));
    std::cout << "[SceneGame] Status window created" << std::endl;

    // ---------------------------------------------------------------
    // レイアウト計算
    // ---------------------------------------------------------------
    const float padding = 5.f;
    float halfWidth = areaScale.x / 2.f;
    float leftCenterX = areaPos.x - halfWidth / 2.f;   // 左側の中心X
    float rightCenterX = areaPos.x + halfWidth / 2.f;  // 右側の中心X
    float rowHeight = areaScale.y / 3.f;               // 左側3段分割
    float topY = areaPos.y + areaScale.y / 2.f;        // エリア上端

    // ---------------------------------------------------------------
    // 右側: UIStatusImage（縦横比維持）
    // ---------------------------------------------------------------
    {
        std::vector<GameObject*> statusImages = objectList->FindGameObjectsWithTag("UIStatusImage");
        if (!statusImages.empty())
        {
            for (size_t i = 0; i < statusImages.size(); i++)
            {
                GameObject* statusImage = statusImages[i];

                // レイヤーセット
                auto* tex = statusImage->GetMeshComponent<Texture2D>();
                tex->SetRenderLayer(RenderLayer::UI_2);

                // 元の縦横比を取得
                Vector3 originalScale = statusImage->GetTransform().GetScale();
                float aspectRatio = originalScale.x / originalScale.y;

                // 右側半分に収まるサイズを計算（縦横比維持）
                float availableWidth = halfWidth - padding * 2.f;
                float availableHeight = areaScale.y - padding * 2.f;

                float finalWidth, finalHeight;
                if (availableWidth / aspectRatio <= availableHeight)
                {
                    // 幅基準
                    finalWidth = availableWidth;
                    finalHeight = availableWidth / aspectRatio;
                }
                else
                {
                    // 高さ基準
                    finalHeight = availableHeight;
                    finalWidth = availableHeight * aspectRatio;
                }

                statusImage->GetTransform().SetPosition(Vector3(rightCenterX, areaPos.y, 0.f));
                statusImage->GetTransform().SetScale(Vector3(finalWidth, finalHeight, 1.f));

                // 一旦すべて非表示
                statusImages[i]->SetActive(false);
            }

            // 仮で１つ目を表示
            // ☆一旦非アクティブ
            statusImages[0]->SetActive(false);
        }
    }
    // ---------------------------------------------------------------
    // 左側2段目: UIStatusJob（1:1、左寄せ）
    // ---------------------------------------------------------------
    {
        std::vector<GameObject*> statusJobs = objectList->FindGameObjectsWithTag("UIStatusJob");
        if (!statusJobs.empty())
        {
            // 1:1の大きさ（行の高さより小さめ）
            //float jobSize = (rowHeight - padding * 2.f) * 0.8;
            float jobSize = (rowHeight - padding * 2.f) * 3;

            // 2段目のY座標（上から2番目）
            float row2Y = topY - rowHeight * 1.5f;

            // 左寄せ（左側エリアの左端からpadding分離す）
            float leftEdgeX = areaPos.x - halfWidth + padding + jobSize / 2.f;

            for (size_t i = 0; i < statusJobs.size(); i++)
            {
                GameObject* statusJob = statusJobs[i];

                // レイヤーセット
                auto* tex = statusJob->GetMeshComponent<Texture2D>();
                tex->SetRenderLayer(RenderLayer::UI_2);

                statusJob->GetTransform().SetPosition(Vector3(leftEdgeX, row2Y, 0.f));
                statusJob->GetTransform().SetScale(Vector3(jobSize, jobSize, 1.f));

                // 一旦すべて非表示
                statusJobs[i]->SetActive(false);
            }

            // 仮で１つ目を表示
            statusJobs[0]->SetActive(true);
        }
    }
    // ---------------------------------------------------------------
    // 左側3段目: UIStatusSpeed x3（中央整列）
    // ---------------------------------------------------------------
    GameObject* speedTemplate = objectList->FindGameObjectWithTag("UIStatusSpeed");
    if (speedTemplate)
    {
        // テンプレートを非表示
        speedTemplate->SetActive(false);
        auto* speedTex = speedTemplate->GetMeshComponent<Texture2D>();
        if (speedTex)
        {
            // 3段目のY座標（上から3番目）
            float row3Y = topY - rowHeight * 2.5f;

            // アイコンサイズ（行の高さに合わせる）
            const int speedCount = 3;
            float iconSize = rowHeight - padding * 2.f;
            float totalWidth = iconSize * speedCount + padding * (speedCount + 1);  // 左右もパディングする

            // 中央整列の開始X座標(左をパディング)
            float startX = leftCenterX - totalWidth / 2.f + iconSize / 2.f + padding;

            for (int i = 0; i < speedCount; i++)
            {
                float posX = startX + i * (iconSize + padding);

                auto speedObj = std::make_unique<GameObject>(
                    Vector3(posX, row3Y, 0.f),
                    Vector3(0, 0, 0),
                    Vector3(iconSize, iconSize, 1.f));
                int id = objectList->GetLastID();
                speedObj->SetID(id);
                speedObj->SetName("UIStatusSpeed" + std::to_string(i + 1));
                speedObj->SetTag("UIStatusSpeedIcon");
                auto* tex = speedObj->AddMeshComponent<Texture2D>(speedTex->GetTexturePath(), Color(1, 1, 1, 1));
                // レイヤーセット
                tex->SetRenderLayer(RenderLayer::UI_2);

                //一旦非アクティブ
                speedObj->SetActive(false);

                objectList->AddObject(std::move(speedObj));
            }
        }
    }

    std::cout << "[SceneGame] Status UI elements arranged" << std::endl;
}

void UISystemComponent::MakeUISunRoute(std::unique_ptr<GameObjectList>& objectList)
{
    // UIAreaStatusを名前で取得
    GameObject* Area = objectList->FindGameObjectWithName("UIAreaSunRoute");
    if (!Area) return;

    // WindowTemplateからテクスチャパスを取得
    GameObject* windowTemplate = objectList->FindGameObjectWithTag("WindowTemplate");
    if (!windowTemplate) return;
    auto* templateTex = windowTemplate->GetMeshComponent<Texture2D>();
    if (!templateTex) return;

    // エリアの位置・スケールを取得
    Vector3 areaPos = Area->GetTransform().GetPosition();
    Vector3 areaScale = Area->GetTransform().GetScale();

    // ステータスウィンドウを作成
    auto windowObj = std::make_unique<GameObject>(areaPos, Vector3(0, 0, 0), areaScale);
    int id = objectList->GetLastID();
    windowObj->SetID(id);
    windowObj->SetName("WindowSunRoute");
    windowObj->SetTag("Window");
    windowObj->AddMeshComponent<Texture2D>(templateTex->GetTexturePath(), Color(1, 1, 1, 1));

    objectList->AddObject(std::move(windowObj));
    std::cout << "[SceneGame] SunRoute window created" << std::endl;
}

ButtonComponent::UIButtonType UISystemComponent::GetHoverButton() const
{
    if(!m_pUIObjects) return ButtonComponent::UIButtonType::None;

    //UI上の全ボタンを取得
    auto buttons = m_pUIObjects->FindGameObjectsWithTag("Button");

    for (auto* btnObj : buttons)
    {
        auto* buttonComp = btnObj->GetComponent<ButtonComponent>();
        if (!buttonComp) continue;

        if (buttonComp->IsSelected())
        {
            return buttonComp->GetButtonType();
        }
    }
    return ButtonComponent::UIButtonType::None;
}

void UISystemComponent::ButtonChange(UnitComponent::UnitModel model)
{
    if (!m_pUIObjects) return;

    auto buttonAttack = m_pUIObjects->FindGameObjectWithName("ButtonAttack");
    auto buttonBuild = m_pUIObjects->FindGameObjectWithName("ButtonBuild");
    auto buttonMove = m_pUIObjects->FindGameObjectWithName("ButtonMove");

    // 移動ボタンは常に表示
    if (buttonMove) buttonMove->SetActive(true);

    // 攻撃／配置切替
    if (model == UnitComponent::UnitModel::UnitPlacementer)
    {
        if (buttonBuild)  buttonBuild->SetActive(true);
        if (buttonAttack) buttonAttack->SetActive(false);
    }
    else if (model == UnitComponent::UnitModel::UnitAttacker)
    {
        if (buttonBuild)  buttonBuild->SetActive(false);
        if (buttonAttack) buttonAttack->SetActive(true);
    }
    else
    {
        if (buttonBuild)  buttonBuild->SetActive(false);
        if (buttonAttack) buttonAttack->SetActive(false);
    }

    // ---------------------------------------------------------------
    // ジョブアイコン(UIStatusJob)切替
    // ---------------------------------------------------------------
    auto jobAttack = m_pUIObjects->FindGameObjectWithName("UIStatusJobAttack");
    auto jobBuild = m_pUIObjects->FindGameObjectWithName("UIStatusJobBuild");

    if (model == UnitComponent::UnitModel::UnitPlacementer)
    {
        if (jobBuild)  jobBuild->SetActive(true);
        if (jobAttack) jobAttack->SetActive(false);
    }
    else if (model == UnitComponent::UnitModel::UnitAttacker)
    {
        if (jobBuild)  jobBuild->SetActive(false);
        if (jobAttack) jobAttack->SetActive(true);
    }
    else
    {
        if (jobBuild)  jobBuild->SetActive(false);
        if (jobAttack) jobAttack->SetActive(false);
    }
}