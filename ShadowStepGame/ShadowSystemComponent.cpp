#include "ShadowSystemComponent.h"

void ShadowSystemComponent::UpdateShadowMap(
    const int* const* mapData,
    const ShadowParam& param,
    GameObjectList* gameObjectList)
{
    //=======================================
       // 初期化
       //=======================================
    ClearShadowMap();

    //=======================================
    // データ更新
    //=======================================
    // ★TODO　巨人など影の大きさが異なるものにも対応する
    for (int z = 0; z < m_MapHeight; ++z)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            // オブジェクトがある場合のみ
            if (mapData[z][x] == 0 ||
                mapData[z][x] == 5 ||
                mapData[z][x] == 99) continue;

            int shadowX = x;
            int shadowZ = z;

            //-----------------------------------
            // 一律方向・一律長さで影を生成
            //-----------------------------------
            for (int i = 0; i < param.length; ++i)
            {
                shadowX += param.lightDirection.x;
                shadowZ += param.lightDirection.z;

                // マップ外なら終了
                if (shadowX < 0 || shadowX >= m_MapWidth ||
                    shadowZ < 0 || shadowZ >= m_MapHeight)
                    break;

                // 空きマスのみ影
                if (mapData[shadowZ][shadowX] == 0)
                {
                    m_ShadowMapData[shadowZ][shadowX] = 1;
                    m_ShadowSourceMap[shadowZ][shadowX] = mapData[z][x]; // 元オブジェクトtypeを入れる
                }
            }
        }
    }

    //=======================================
    // 影オブジェクトの表示/Scale/Positionを更新
    //=======================================
    UpdateShadowObjects(mapData, param, gameObjectList);
}

ShadowParam ShadowSystemComponent::CalcShadowParm(
    MapPosition lightDirection,
    int mapWidth, int mapHeight)
{
    ShadowParam param{};

    param.lightDirection = lightDirection;

    //=======================================
    // 真上（影なし）
    //=======================================
    if (lightDirection.x == 0 && lightDirection.z == 0)
    {
        param.length = 0;
        return param;
    }

    //=======================================
    // 影方向（十字4方向）
    //=======================================


    //=======================================
    // 影の長さ（距離感）
    //=======================================
    int distance =
        abs(lightDirection.x) + abs(lightDirection.z);   // マンハッタン距離

    param.length = std::clamp(
        distance / 2,   // SHADOW_DIV
        2,              // MIN_SHADOW
        8               // MAX_SHADOW
    );

    return param;
}

void ShadowSystemComponent::MakeShadowMap()
{
    m_ShadowMapData = new int* [m_MapHeight];
    m_ShadowSourceMap = new int* [m_MapHeight];
    for (int z = 0; z < m_MapHeight; ++z)
    {
        m_ShadowMapData[z] = new int[m_MapWidth];
        m_ShadowSourceMap[z] = new int[m_MapWidth];
    }
    ClearShadowMap();
}

void ShadowSystemComponent::ClearShadowMap()
{
    for (int z = 0; z < m_MapHeight; ++z)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            m_ShadowMapData[z][x] = 0;
            m_ShadowSourceMap[z][x] = 0;
        }
    }
}

void ShadowSystemComponent::CreateShadowObjects(const int* const* mapData, GameObjectList* gameObjectList)
{
    if (!gameObjectList) return;
    if (!mapData) return;

    // ShadowTemplateを取得
    GameObject* shadowTemplate = gameObjectList->FindGameObjectWithTag("ShadowTemplate");
    if (!shadowTemplate)
    {
        std::cout << "[ShadowSystem] ShadowTemplateが見つかりません！" << std::endl;
        return;
    }

    // テンプレートからテクスチャパスを取得
    auto* templateRenderer = shadowTemplate->GetMeshComponent<Texture2D>();
    if (templateRenderer)
    {
        m_ShadowTexturePath = templateRenderer->GetTexturePath();
    }

    // テンプレートを非表示
    shadowTemplate->SetActive(false);

    int cnt = 0;

    for (int z = 0; z < m_MapHeight; ++z)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            // 非生成オブジェクト以外は生成しない
            if (mapData[z][x] == (int)EMapTile::Empty || mapData[z][x] == (int)EMapTile::Shadow || mapData[z][x] == (int)EMapTile::None)
                continue;

            // 元オブジェクトの位置計算
            float posX = m_DrawStartPosX + x * m_SizePiece;
            float posZ = m_DrawStartPosZ + z * m_SizePiece;

            // オブジェクト生成（初期状態では基本サイズ）
            auto obj = std::make_unique<GameObject>(
                Vector3(posX, m_ShadowHeight, posZ),
                Vector3::Zero,
                Vector3(m_SizePiece, 1.0f, m_SizePiece)
            );
            GameObject* newObject = obj.get();
            newObject->SetID(m_LastObjectId++);
            newObject->SetName("Shadow");
            newObject->SetTag("Shadow");

            // レンダラー追加（SimplePlaneRendererComponentは3D用）
            Color shadowColor = Color(0.0f, 0.0f, 0.0f, 0.5f);
            newObject->AddMeshComponent<SimplePlaneRendererComponent>(shadowColor, m_ShadowTexturePath);

            gameObjectList->AddObject(std::move(obj));
            cnt++;
        }
    }

    std::cout << "[ShadowSystem] 影オブジェクトを" << cnt << "個生成しました" << std::endl;
}

void ShadowSystemComponent::UpdateShadowObjects(const int* const* mapData, const ShadowParam& param, GameObjectList* gameObjectList)
{
    // 影の方向（光源の反対方向）
    int shadowDirX = -param.lightDirection.x;
    int shadowDirZ = -param.lightDirection.z;

    int shadowCnt = 0;

    std::vector<GameObject*> shadowObjects = gameObjectList->FindGameObjectsWithTag("Shadow");
    // すべて非表示
    for (auto& object : shadowObjects)
    {
        object->SetActive(false);
    }

    for (int z = 0; z < m_MapHeight; ++z)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            // オブジェクトありの場合
            if (mapData[z][x] != 0)
            {
                // 影がオブジェクトが足りない場合は追加
                if (shadowObjects.size() < shadowCnt)
                {
                    // オブジェクト生成（初期状態では基本サイズ）
                    auto obj = std::make_unique<GameObject>(
                        Vector3(0.f, m_ShadowHeight, 0.f),  // 位置はあとで変えるので仮
                        Vector3::Zero,
                        Vector3(m_SizePiece, 1.0f, m_SizePiece)
                    );
                    GameObject* newObject = obj.get();
                    newObject->SetID(m_LastObjectId++);
                    newObject->SetName("Shadow");
                    newObject->SetTag("Shadow");

                    // レンダラー追加（SimplePlaneRendererComponentは3D用）
                    Color shadowColor = Color(0.0f, 0.0f, 0.0f, 0.5f);
                    newObject->AddMeshComponent<SimplePlaneRendererComponent>(shadowColor, m_ShadowTexturePath);
                }

                GameObject* shadowObj = shadowObjects[shadowCnt];

                // 表示
                shadowObj->SetActive(true);

                // 元オブジェクトの描画位置
                float baseX = m_DrawStartPosX + x * m_SizePiece;
                float baseZ = m_DrawStartPosZ + z * m_SizePiece;

                float sizeShadow = m_SizePiece * 0.5f;

                // スケール計算（伸びる方向にLength分拡大）
                float scaleX = sizeShadow;
                float scaleZ = sizeShadow;

                if (shadowDirX != 0)
                {
                    scaleX = sizeShadow * param.length;
                }
                if (shadowDirZ != 0)
                {
                    scaleZ = sizeShadow * param.length;
                }

                // 位置計算（影の中心を伸びる方向にオフセット）
                float offsetX = shadowDirX * (param.length * sizeShadow / 2.0f);
                float offsetZ = shadowDirZ * (param.length * sizeShadow / 2.0f);

                float posX = baseX + offsetX;
                float posZ = baseZ + offsetZ;

                // Transform更新
                shadowObj->GetTransform().SetPosition(Vector3(posX, m_ShadowHeight, posZ));
                shadowObj->GetTransform().SetScale(Vector3(scaleX, 1.0f, scaleZ));

                shadowCnt++;
            }
        }
    }
}
