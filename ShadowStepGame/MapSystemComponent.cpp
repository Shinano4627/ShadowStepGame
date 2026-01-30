#include "MapSystemComponent.h"

#include <fstream>
#include <sstream>
#include <stdio.h>

#include "SimplePlaneRendererComponent.h"
#include "UnitCommon.h"
#include "GameObjectList.h"
#include "MeshRendererComponent.h"
#include "SimpleCubeRendererComponent.h"

using namespace std;

void MapSystemComponent::MakeMap(std::unique_ptr<GameObjectList>& objectList)      // CSVデータ読み込みとマップオブジェクトの作成
{
    int mapZ = 0;
    int mapX = 0;
    int n = 3000;
    ifstream csv_data(m_DataFile, ios::in);

    // 既存データの削除
    DeleteMap();

    if (!csv_data.is_open())
    {
        cout << "Error: opening file fail" << endl;
        exit(1);
    }
    else
    {
        cout << "Start Read : " << m_DataFile << endl;

        string line;

        vector<string> words; //文字列ベクトルを宣言する
        string word;
        // ------------データ読み取り-----------------
        istringstream sin;
        // マップの広さを取得
        getline(csv_data, line);
        sin.clear();
        sin.str(line);
        //文字列ストリームsinの文字をコンマ区切り
        getline(sin, word, ',');
        m_MapWidth = stoi(word);
        getline(sin, word, ',');
        m_MapHeight = stoi(word);

        // マップインスタンス作成
        m_MapData = new int* [m_MapHeight]();
        for (int i = 0; i < m_MapHeight; i++)
        {
            m_MapData[i] = new int[m_MapWidth]();
        }
        m_UnitMapData = new int* [m_MapHeight]();
        for (int i = 0; i < m_MapHeight; i++)
        {
            m_UnitMapData[i] = new int[m_MapWidth]();
        }
        m_ObjectMapData = new int* [m_MapHeight]();
        for (int i = 0; i < m_MapHeight; i++)
        {
            m_ObjectMapData[i] = new int[m_MapWidth]();
        }

        // 原点を中心に表示されるようにスタート位置を計算
        m_DrawStartPosX = -m_MapWidth * m_SizePiece / 2.f + m_SizePiece / 2.f;
        m_DrawStartPosZ = -m_MapHeight * m_SizePiece / 2.f + m_SizePiece / 2.f;

        // 行ごとにデータを読み込む
        while (getline(csv_data, line)) {
            // vectorおよび文字ストリームをクリアし、前の行 のデータのみを保存します
            words.clear();
            sin.clear();
            sin.str(line);
            //文字列ストリームsinの文字をコンマ区切り文字列配列wordsに配置する
            while (getline(sin, word, ',')) {
                //cout << word << endl;
                words.push_back(word); //セル内のデータを1つずつpushする
            }

            //行に従ったmapの作成を開始
            for (string str : words)
            {
                // トランスフォームデータを渡す
                auto obj = std::make_unique<GameObject>
                    (Vector3(m_DrawStartPosX + mapX * m_SizePiece, m_DrawStartPosY, m_DrawStartPosZ + mapZ * m_SizePiece), Vector3::Zero, Vector3(m_SizePiece / 2, 1.f, m_SizePiece / 2));
                GameObject* newObject = obj.get();
                newObject->SetID(n);
                newObject->SetName("Map");
                newObject->SetTag("Map");

                int data = stoi(str);
                m_MapData[mapZ][mapX] = data;

                // CSVからのよみとり
                Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
                switch ((EMapTile)data)
                {
                case EMapTile::Wall:
                    //壁
                    color = Color(0.2f, 0.2f, 0.2f, 1.0f);
                    break;
                case EMapTile::Tree:
                    //樹
                    color = Color(0, 1.0f, 0, 1.0f);
                    break;
                case EMapTile::Empty: //何もない
                case EMapTile::Player: //プレーヤー
                case EMapTile::Enemy: //敵
                case EMapTile::Shadow: //影
                    color = Color(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                
                    break;
                default:

                    break;
                }
                newObject->AddMeshComponent<SimplePlaneRendererComponent>(color, "asset/texture/Grid/Base.png"); // TODO　Templateから取得するようにしたい
                mapX++;
                n++;
                objectList->AddObject(std::move(obj));
            }
            mapX = 0;
            mapZ++;
            //return words;
        }

        csv_data.close();

        // レイヤーデータの作成
        for (int i = 0; i < m_MapHeight; i++)
        {
            for (int j = 0; j < m_MapWidth; j++)
            {
                switch ((EMapTile)m_MapData[i][j])
                {
                    // オブジェクト
                case EMapTile::Wall:
                    m_UnitMapData[i][j] = (int)EMapTile::Empty;
                    m_ObjectMapData[i][j] = (int)EMapTile::Wall;
                    break;
                case EMapTile::Tree:
                    m_UnitMapData[i][j] = (int)EMapTile::Empty;
                    m_ObjectMapData[i][j] = (int)EMapTile::Tree;
                    break;
                    // ユニット
                case EMapTile::Player:
                    m_UnitMapData[i][j] = (int)EMapTile::Player;
                    m_ObjectMapData[i][j] = (int)EMapTile::Empty;
                    break;
                case EMapTile::Enemy:
                    m_UnitMapData[i][j] = (int)EMapTile::Enemy;
                    m_ObjectMapData[i][j] = (int)EMapTile::Empty;
                    break;
                    // その他
                case EMapTile::Shadow:
                case EMapTile::Empty:
                case EMapTile::None:
                    m_UnitMapData[i][j] = (int)EMapTile::Empty;
                    m_ObjectMapData[i][j] = (int)EMapTile::Empty;
                    break;
                default:
                    break;
                }

                // マップ表示用オブジェクト作成
                MakeMapObjectData(objectList.get(), GetPositionToMap(MapPosition(j, i)), (EMapTile)m_MapData[i][j]);
                std::cout << m_MapData[i][j];       // デバッグ出力
            }
            std::cout << std::endl;
        }

        // テンプレートオブジェクトを非表示 TODO：タグをテンプレートで統一して一括で非表示にする
        objectList->FindGameObjectWithName("EnemyTemplate")->SetActive(false);
        objectList->FindGameObjectWithName("PlayerAttackTemplate")->SetActive(false);
        objectList->FindGameObjectWithName("PlayerPlaceTemplate")->SetActive(false);
        objectList->FindGameObjectWithName("TreeTemplate")->SetActive(false);
    }

}

void MapSystemComponent::DeleteMap()
{
    if (m_MapData)
    {
        for (int i = 0; i < m_MapHeight; i++)
        {
            delete[] m_MapData[i];
        }
        delete[] m_MapData;
    }

    if (m_UnitMapData)
    {
        for (int i = 0; i < m_MapHeight; i++)
        {
            delete[] m_UnitMapData[i];
        }
        delete[] m_UnitMapData;
    }

    if (m_ObjectMapData)
    {
        for (int i = 0; i < m_MapHeight; i++)
        {
            delete[] m_ObjectMapData[i];
        }
        delete[] m_ObjectMapData;
    }

    if (m_SelectMapData)
    {
        for (int z = 0; z < m_MapHeight; z++)
            delete[] m_SelectMapData[z];
        delete[] m_SelectMapData;
    }

    if (m_SelectMapObjects)
    {
        for (int z = 0; z < m_MapHeight; z++)
            delete[] m_SelectMapObjects[z];
        delete[] m_SelectMapObjects;
    }
}

// ===================================================================
// GameSystemで行うMap更新処理
// UnitData,ShadowData,地形MapDataを元にMapDataを更新する
// ===================================================================
void MapSystemComponent::UpdateMap(const std::vector<UnitComponent*>& units,
    const int* const* shadowMap,
    GameObjectList* gameObjectList)
{
    if (!m_pOwner) return;

    // クリア
    for (int i = 0; i < m_MapHeight; i++)
    {
        for (int j = 0; j < m_MapWidth; j++)
        {
            m_MapData[i][j] = (int)EMapTile::Empty;
            m_UnitMapData[i][j] = (int)EMapTile::Empty;
        }
    }

    //=======================================
    // UnitSystemからプレイヤー・敵の位置を取得
    //=======================================
    for(auto* unit : units)
    {
        if (unit->IsDown()) continue;
        
        MapPosition u_map = unit->GetPosition();

        // 位置情報取得・反映
        int mapX, mapZ;
        // UnitPositionからMap内Positionに変換
        if (!ConvertUnitPosToMapIndex(u_map.x, u_map.z, mapX, mapZ))
            continue;

        m_UnitMapData[mapZ][mapX] =
            (unit->GetType() == UnitType::Player)
            ? (int)EMapTile::Player
            : (int)EMapTile::Enemy;
    }

    //=======================================
    // ユニット＞オブジェクト＞影になるように結合
    //=======================================
    
    for (int z = 0; z < m_MapHeight; ++z)
    {
        for (int x = 0; x < m_MapWidth; ++x)
        {
            // 影
            if (shadowMap[z][x] != 0)
            {
                m_MapData[z][x] = (int)EMapTile::Shadow;
            }

            if (m_ObjectMapData[z][x] != (int)EMapTile::Empty)
            {
                m_MapData[z][x] = m_ObjectMapData[z][x];
            }

            if (m_UnitMapData[z][x] != (int)EMapTile::Empty)
            {
                m_MapData[z][x] = m_UnitMapData[z][x];
            }

            std::cout << m_MapData[z][x];       // デバッグ出力
        }
        std::cout << std::endl;
    }
    
}
// ===================================================================
// 生マップデータ取得
// ===================================================================
const int* const* MapSystemComponent::GetRawMapData() const
{
    return m_MapData;
}

// 変換関数　Unit → Map 
bool MapSystemComponent::ConvertUnitPosToMapIndex(
    int unitX, int unitZ,
    int& outMapX, int& outMapZ) const
{
    outMapX = unitX + m_MapWidth / 2;
    outMapZ = unitZ + m_MapHeight / 2;

    if (outMapX < 0 || outMapX >= m_MapWidth ||
        outMapZ < 0 || outMapZ >= m_MapHeight)
    {
        return false;
    }
    return true;
}

// 変換関数　Map → Unit
bool MapSystemComponent::ConvertMapIndexToUnitPos(
    int mapX, int mapZ,
    int& outUnitX, int& outUnitZ) const
{
    outUnitX = mapX - m_MapWidth / 2;
    outUnitZ = mapZ - m_MapHeight / 2;

    return true;
}

// Unit座標からMapの中身を調べて返す
EMapTile MapSystemComponent::GetTileAtUnitPos(int unitX, int unitZ) const
{
    int mapX, mapZ;
    if (!ConvertUnitPosToMapIndex(unitX, unitZ, mapX, mapZ))
    {
        return EMapTile::None; // マップ外
    }

    return static_cast<EMapTile>(m_MapData[mapZ][mapX]);
}

// ユニットが歩けるマスか？
bool MapSystemComponent::IsWalkableAtUnitPos(int unitX, int unitZ) const
{
    EMapTile tile = GetTileAtUnitPos(unitX, unitZ);

    // Empty,Shadow ＝ 歩けるマス
    return tile == EMapTile::Empty
        || tile == EMapTile::Shadow;
}

// ユニットが攻撃できるマスか？
bool MapSystemComponent::IsAttackableAtUnitPos(int fromX, int fromZ,
    int toX, int toZ, UnitType type) const
{
    EMapTile tile = GetTileAtUnitPos(toX, toZ);
    int dx = abs(fromX - toX);
    int dz = abs(fromZ - toZ);
    switch (type)
    {
    case UnitType::Enemy:
        if (tile != EMapTile::Player) return false;
        // 縦横指定マス以内
        
        return (dx + dz) <= 2 && (dx == 0 || dz == 0);
        break;
    case UnitType::Player:
        if (tile != EMapTile::Enemy) return false;
        // 縦横指定マス以内
        
        return (dx + dz) <= 2 && (dx == 0 || dz == 0);
        break;
    default:
        return false;
    }
}

// ユニットが配置できるマスか？
bool MapSystemComponent::IsPlacebleAtUnitPos(int x, int z) const
{
    return GetTileAtUnitPos(x, z) == EMapTile::Empty;
}


void MapSystemComponent::MakeSelectMap(std::unique_ptr<GameObjectList>& objectList)
{
    if (!m_SelectMapData)
    {
        m_SelectMapData = new int* [m_MapHeight];
        for (int z = 0; z < m_MapHeight; z++)
            m_SelectMapData[z] = new int[m_MapWidth] {};
    }
    // SelectMapObjects
    if (!m_SelectMapObjects)
    {
        m_SelectMapObjects = new GameObject * *[m_MapHeight];
        for (int z = 0; z < m_MapHeight; z++)
            m_SelectMapObjects[z] = new GameObject * [m_MapWidth] {};
    }

    int n = 4000;   // ダブらないようSelectMap用ObjectID

    // SelectMapDataの作成
    for (int mapZ = 0; mapZ < m_MapHeight; mapZ++) {
        for (int mapX = 0; mapX < m_MapWidth; mapX++) {
            // トランスフォームを渡す(pos,rot,scl)
            auto obj = std::make_unique<GameObject>
                (Vector3(m_DrawStartPosX + mapX * m_SizePiece, 0.21f, m_DrawStartPosZ + mapZ * m_SizePiece),
                    Vector3::Zero, Vector3(m_SizePiece / 2, 1.f, m_SizePiece / 2));
            GameObject* newObject = obj.get();
            newObject->SetID(n);
            newObject->SetName("SelectMap");
            newObject->SetTag("SelectMap");
            // NULLをセット
            m_SelectMapData[mapZ][mapX] = 0;
            // MeshComponentをAdd
            Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
            newObject->AddMeshComponent<SimplePlaneRendererComponent>(color);
            obj->SetActive(false);
            n++;
            objectList->AddObject(std::move(obj));
            m_SelectMapObjects[mapZ][mapX] = newObject;
        }
    }

    // SelectUIの作成
    // トランスフォームを渡す(pos,rot,scl)
    auto obj = std::make_unique<GameObject>
        (Vector3(m_DrawStartPosX, 0.22f, m_DrawStartPosZ),
            Vector3::Zero, Vector3(m_SizePiece / 2, 1.f, m_SizePiece / 2));
    GameObject* newObject = obj.get();
    newObject->SetID(n);
    newObject->SetName("SelectUI");
    newObject->SetTag("SelectUI");
    // MeshComponentをAdd
    Color color = Color(1.0f, 1.0f, 1.0f, 0.8f);
    newObject->AddMeshComponent<SimplePlaneRendererComponent>(
        color);
    obj->SetActive(false);
    n++;
    objectList->AddObject(std::move(obj));
    m_pSelectCursor = newObject;
}

void MapSystemComponent::StartSelectMap(UnitComponent* unit,MapPosition selectpos)
{
    if (!unit) return;

    m_IsSelectMapActive = true;

    // SelectMap を改めて非アクティブ化
    for (int z = 0; z < m_MapHeight; z++)
        for (int x = 0; x < m_MapWidth; x++)
            m_SelectMapObjects[z][x]->SetActive(false);

    // UnitPosをMapPositionからMap内Positionに変換
    MapPosition unit_pos = unit->GetPosition();
    MapPosition map_unitpos;
    ConvertUnitPosToMapIndex(unit_pos.x, unit_pos.z, map_unitpos.x, map_unitpos.z);
    
    // Unitに応じた範囲計算
    // ==============================
    // SelectMapData 初期化（全マス None）
    // ==============================
    for (int z = 0; z < m_MapHeight; z++)
    {
        for (int x = 0; x < m_MapWidth; x++)
        {
            m_SelectMapData[z][x] = static_cast<int>(SMapTile::Empty);
            std::cout << m_SelectMapData[z][x];
        }
    }
    
    // ==============================
    // 配置範囲（5x5 = 緑）
    // ==============================
    if (unit->GetModel() == UnitModel::Place) {
        for (int dz = -2; dz <= 2; dz++)
        {
            for (int dx = -2; dx <= 2; dx++)
            {
                int x = map_unitpos.x + dx;
                int z = map_unitpos.z + dz;

                if (x < 0 || x >= m_MapWidth ||
                    z < 0 || z >= m_MapHeight)
                    continue;

                m_SelectMapData[z][x] = static_cast<int>(SMapTile::Place);
            }
        }
    }
    // ==============================
    // 移動範囲（3x3 = 青）
    // ==============================
    for (int dz = -1; dz <= 1; dz++)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int x = map_unitpos.x + dx;
            int z = map_unitpos.z + dz;

            if (x < 0 || x >= m_MapWidth ||
                z < 0 || z >= m_MapHeight)
                continue;

            m_SelectMapData[z][x] = static_cast<int>(SMapTile::Move);
        }
    }

    // ==============================
    // 攻撃範囲（十字5マス = 赤）
    // ==============================
    if (unit->GetModel() == UnitModel::Attack) {
        const int attackOffset[5][2] =
        {
            { 0,  0},
            { 1,  0},
            {-1,  0},
            { 0,  1},
            { 0, -1}
        };

        for (int i = 0; i < 5; i++)
        {
            int x = map_unitpos.x + attackOffset[i][0];
            int z = map_unitpos.z + attackOffset[i][1];

            if (x < 0 || x >= m_MapWidth ||
                z < 0 || z >= m_MapHeight)
                continue;

            m_SelectMapData[z][x] = static_cast<int>(SMapTile::Attack);
        }
    }
    // ==============================
    // SelectMap 表示 & 色反映
    // ==============================
    for (int z = 0; z < m_MapHeight; z++)
    {
        for (int x = 0; x < m_MapWidth; x++)
        {
            int index = z * m_MapWidth + x;
            auto* obj = m_SelectMapObjects[z][x];
            if (!obj) continue;

            switch (m_SelectMapData[z][x])
            {
            case static_cast<int>(SMapTile::Attack):
                obj->SetActive(true);
                obj->GetMeshComponent<SimplePlaneRendererComponent>()
                    ->SetColor(Color(1.0f,0.0f,0.0f, 0.4f));
                break;

            case static_cast<int>(SMapTile::Move):
                obj->SetActive(true);
                obj->GetMeshComponent<SimplePlaneRendererComponent>()
                    ->SetColor(Color(0.0f, 0.0f, 1.0f, 0.4f));
                break;

            case static_cast<int>(SMapTile::Place):
                obj->SetActive(true);
                obj->GetMeshComponent<SimplePlaneRendererComponent>()
                    ->SetColor(Color(0.0f, 1.0f, 0.0f, 0.4f));
                break;

            default:
                obj->SetActive(false);
                break;
            }
        }
    }

    // カーソル有効化
    if (m_pSelectCursor)
    {
        m_pSelectCursor->SetActive(true);

        // ユニット位置のSelectMapObjを取得
        GameObject* cellObj = m_SelectMapObjects[map_unitpos.z][map_unitpos.x];

        if (cellObj) {
            Vector3 pos = cellObj->GetTransform().GetPosition();
            m_pSelectCursor->GetTransform().SetPosition(
                Vector3(pos.x, pos.y + 0.01f, pos.z)
            );
        }
    }

}

void MapSystemComponent::UpdateSelectCursor(MapPosition selectpos)
{
    if (!m_IsSelectMapActive) return;
    if (!m_pSelectCursor) return;

    int mapX, mapZ;

    // Unit基準 → Map配列に変換
    if (!ConvertUnitPosToMapIndex(
        selectpos.x,
        selectpos.z,
        mapX,
        mapZ))
    {
        return; // マップ外
    }

    // 範囲外ガード（念のため）
    if (mapX < 0 || mapX >= m_MapWidth ||
        mapZ < 0 || mapZ >= m_MapHeight)
    {
        return;
    }

    GameObject* cellObj = m_SelectMapObjects[mapZ][mapX];
    if (!cellObj) return;

    // 対象マスの位置を取得
    Vector3 pos = cellObj->GetTransform().GetPosition();

    // カーソル位置更新（少し上に）
    m_pSelectCursor->GetTransform().SetPosition(
        Vector3(pos.x, pos.y + 0.01f, pos.z)
    );

}

void MapSystemComponent::EndSelectMap()
{
    // SelectMap を改めて非アクティブ化
    for (int z = 0; z < m_MapHeight; z++)
        for (int x = 0; x < m_MapWidth; x++)
            m_SelectMapObjects[z][x]->SetActive(false);

    m_pSelectCursor->SetActive(false);

    m_IsSelectMapActive = false;

}

void MapSystemComponent::MakeMapObjectData(GameObjectList* objectList, const Vector3& pos, EMapTile type)
{
    // オブジェクト生成（初期状態では基本サイズ）
   auto newObject = std::make_unique<GameObject>(
       pos,
        Vector3::Zero,
        Vector3(m_SizePiece * 0.5, m_SizePiece * 0.5, m_SizePiece * 0.5)
    );
   int id = objectList->GetLastID();
   newObject->SetID(id++);
   Transform& transform = newObject->GetTransform();

   // タイプごとにテンプレート取得
   switch (type)
   {
   case EMapTile::Wall:
   {
       newObject->AddMeshComponent<SimpleCubeRendererComponent>(Color(0.3f, 0.3f, 0.3f, 1.f));  // 灰色のキューブ
       newObject->SetName("Wall");
       newObject->SetTag("Wall");
       // スケール分位置を変更
       Vector3 newPos = transform.GetPosition();
       newPos.y = transform.GetScale().y * 0.5;
       transform.SetPosition(newPos);
   }
       break;
   case EMapTile::Player:
   {
       // テンプレートからモデル情報を取得
       auto playerTemplate = objectList->FindGameObjectWithName("PlayerAttackTemplate");
       if (!playerTemplate) return;
       auto* templateMesh = playerTemplate->GetMeshComponent<MeshRendererComponent>();
       if (!templateMesh) return;

       newObject->SetName("Player");
       newObject->SetTag("Player");
       Vector3 newScale = transform.GetScale() * playerTemplate->GetTransform().GetScale();
       transform.SetScale(newScale);

       // メッシュコンポーネントを追加
       auto mesh = newObject->AddMeshComponent<MeshRendererComponent>(
           templateMesh->GetModelPath(),
           templateMesh->GetTexturePath()
       );
       mesh->LoadModel();
   }
       break;
   case EMapTile::Enemy:
   {
       // テンプレートからモデル情報を取得
       auto enemyTemplate = objectList->FindGameObjectWithName("EnemyTemplate");
       if (!enemyTemplate) return;
       auto* templateMesh = enemyTemplate->GetMeshComponent<MeshRendererComponent>();
       if (!templateMesh) return;

       newObject->SetName("Enemy");
       newObject->SetTag("Enemy");
       Vector3 newScale = transform.GetScale() * enemyTemplate->GetTransform().GetScale();
       transform.SetScale(newScale);

       // メッシュコンポーネントを追加
       auto mesh = newObject->AddMeshComponent<MeshRendererComponent>(
           templateMesh->GetModelPath(),
           templateMesh->GetTexturePath()
       );
       mesh->LoadModel();
   }
       break;
   case EMapTile::Tree:
   {
       // テンプレートからモデル情報を取得
       auto treeTemplate = objectList->FindGameObjectWithName("TreeTemplate");
       if (!treeTemplate) return;
       auto* templateMesh = treeTemplate->GetMeshComponent<MeshRendererComponent>();
       if (!templateMesh) return;

       newObject->SetName("Tree");
       newObject->SetTag("Tree");
       Vector3 newScale = transform.GetScale() * treeTemplate->GetTransform().GetScale();
       transform.SetScale(newScale);

       // メッシュコンポーネントを追加
       auto mesh = newObject->AddMeshComponent<MeshRendererComponent>(
           templateMesh->GetModelPath(),
           templateMesh->GetTexturePath()
       );
       mesh->LoadModel();
   }
       break;
   case EMapTile::Shadow:
   case EMapTile::Empty:
   case EMapTile::None:
   default:
       return;  // 終了
   }

    objectList->AddObject(std::move(newObject));    // リスト追加
}
