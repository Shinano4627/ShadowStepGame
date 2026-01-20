#include "MapSystemComponent.h"

#include <fstream>
#include <sstream>
#include <stdio.h>

#include "SimplePlaneRendererComponent.h"

using namespace std;

void MapSystemComponent::MakeMap(std::vector<std::unique_ptr<GameObject>>& objectList)      // CSVデータ読み込みとマップオブジェクトの作成
{
    int mapZ = 0;
    int mapX = 0;
    int n = 3000;
    ifstream csv_data(m_DataFile, ios::in);

    if (!csv_data.is_open()) {
        cout << "Error: opening file fail" << endl;
        exit(1);
    }
    else {
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

        // マップ
        m_MapData = new int*[m_MapHeight]();
        for (int i = 0; i < m_MapHeight; i++)
        {
            m_MapData[i] = new int [m_MapWidth]();
        }

        // 原点を中心に表示されるようにスタート位置を計算
        m_DrawStartPosX = - m_MapWidth * m_SizePiece / 2.f + m_SizePiece / 2.f;
        m_DrawStartPosZ = - m_MapHeight * m_SizePiece / 2.f + m_SizePiece / 2.f;

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
                    (Vector3(m_DrawStartPosX + mapX * m_SizePiece, 0.2f, m_DrawStartPosZ + mapZ * m_SizePiece), Vector3::Zero, Vector3(m_SizePiece/2, 1.f, m_SizePiece / 2));
                GameObject* newObject = obj.get();
                newObject->SetID(n);
                newObject->SetName("Map");
                newObject->SetTag("Map");

                int data = stoi(str);
                m_MapData[mapZ][mapX] = data;

                // CSVからのよみとり
                Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);
                switch (data)
                {
                case 0:
                    //何もない
                    color = Color(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case 1:
                    //壁
                    color = Color(0.2f, 0.2f, 0.2f, 1.0f);
                    break;
                case 2:
                    //プレーヤー
                    color = Color(0, 0, 1.0f, 1.0f);
                    break;
                case 3:
                    //敵
                    color = Color(1.0f, 0, 0, 1.0f);
                    break;
                case 4:
                    //樹
                    color = Color(0, 1.0f, 0, 1.0f);
                    break;
                default:
                    break;
                }
                newObject->AddMeshComponent<SimplePlaneRendererComponent>(color);
                mapX++;
                n++;
                objectList.push_back(std::move(obj));
            }
            mapX = 0;
            mapZ++;
            //return words;
        }

        csv_data.close();
    }

    // デバッグ出力
    for (int i = 0; i < m_MapHeight; i++)
    {
        for (int j = 0; j < m_MapWidth; j++)
        {
            std::cout << m_MapData[i][j];
        }
        std::cout << std::endl;
    }
    
}