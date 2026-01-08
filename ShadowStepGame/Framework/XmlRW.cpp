//=======================================
// XmlRW.cpp
// xmlファイルを読み込み各SceneのGameObjectDataを作成する
//=======================================
#include "XmlRW.h"

#include <iostream>
#include <unordered_map>
#include "download/tinyxml2.h"

using namespace tinyxml2;

//float GetFloatAttribute(XMLElement* elem, const char* attrName, float defaultValue = 0.0f);
float GetFloatAttribute(XMLElement* elem, const char* attrName, float defaultValue = 0.0f)
{
    const char* attr = elem->Attribute(attrName);
    if (attr) return static_cast<float>(atof(attr));
    return defaultValue;
}

std::string compose_file_name(const char* _stage)
{
    std::string xmlFile = "data/";
    xmlFile.append(_stage);
    xmlFile.append(".xml");
    return xmlFile;
}

int XmlRW::GetObjectData(const char* _stage, std::vector<ObjectData>& _objectList)
{
    XMLDocument doc;
    if (doc.LoadFile(compose_file_name(_stage).c_str()) != XML_SUCCESS) {
        std::cout << "Failed to load XML\n";
        return -1;
    }

    XMLElement* root = doc.FirstChildElement("Objects");
    if (!root) {
        std::cout << "No <Objects> element!\n";
        return -1;
    }

    for (XMLElement* elem = root->FirstChildElement("Object"); elem != nullptr; elem = elem->NextSiblingElement("Object"))
    {
        ObjectData data;

        elem->QueryIntAttribute("ID", &data.id);

        // -----------------------------
        // Value属性取得用
        // -----------------------------
        auto GetValue = [&](const char* tag) -> std::string {
            XMLElement* e = elem->FirstChildElement(tag);
            return e && e->Attribute("Value") ? e->Attribute("Value") : "";
            };

        data.objectName = GetValue("ObjectName");
        data.objectType = GetValue("ObjectType");
        data.objectTag = GetValue("Tag");
        data.fileName = GetValue("FileName");
        data.texture = GetValue("Texture");

         //-----------------------------
         //Animations（可変個）
         //-----------------------------
        XMLElement* animsElem = elem->FirstChildElement("Animations");
        if (animsElem)
        {
            for (XMLElement* animElem = animsElem->FirstChildElement("Animation");
                animElem;
                animElem = animElem->NextSiblingElement("Animation"))
            {
                AnimationData anim{};
                if (animElem->Attribute("Name"))
                    anim.name = animElem->Attribute("Name");
                if (animElem->Attribute("Path"))
                    anim.path = animElem->Attribute("Path");

                data.animations.push_back(anim);
            }
        }

        // -----------------------------
        // Transform / Color
        // -----------------------------
        XMLElement* pos = elem->FirstChildElement("Posision");
        if (pos) {
            pos->QueryFloatAttribute("X", &data.pos[0]);
            pos->QueryFloatAttribute("Y", &data.pos[1]);
            pos->QueryFloatAttribute("Z", &data.pos[2]);
        }

        XMLElement* rot = elem->FirstChildElement("Rotation");
        if (rot) {
            rot->QueryFloatAttribute("X", &data.rot[0]);
            rot->QueryFloatAttribute("Y", &data.rot[1]);
            rot->QueryFloatAttribute("Z", &data.rot[2]);
        }

        XMLElement* scl = elem->FirstChildElement("Scale");
        if (scl) {
            scl->QueryFloatAttribute("X", &data.scl[0]);
            scl->QueryFloatAttribute("Y", &data.scl[1]);
            scl->QueryFloatAttribute("Z", &data.scl[2]);
        }

        XMLElement* col = elem->FirstChildElement("Color");
        if (col) {
            col->QueryFloatAttribute("R", &data.color[0]);
            col->QueryFloatAttribute("G", &data.color[1]);
            col->QueryFloatAttribute("B", &data.color[2]);
            col->QueryFloatAttribute("A", &data.color[3]);
        }

        _objectList.push_back(data);
    }

    return 0;
}

void XmlRW::UpdateObjectsInXML(const std::vector<ObjectData>& objects, const char* _stage)
{
    XMLDocument doc;

    std::string fileName = compose_file_name(_stage);
    // XML の読み込み（既存ファイル優先）
    if (doc.LoadFile(fileName.c_str()) != XML_SUCCESS) {
        // ファイルが無い場合は新規作成
        doc.InsertFirstChild(doc.NewElement("Objects"));
    }

    XMLElement* root = doc.FirstChildElement("Objects");
    if (!root) {
        root = doc.NewElement("Objects");
        doc.InsertFirstChild(root);
    }

    // ObjectName をキーとして扱うため、既存 Object をマップにする
    std::unordered_map<std::string, XMLElement*> existingMap;

    for (XMLElement* elem = root->FirstChildElement("Object"); elem; elem = elem->NextSiblingElement("Object"))
    {
        const char* name = elem->Attribute("ObjectName");
        if (name) {
            existingMap[name] = elem;
        }
    }

    // ----------------------------------------
    // newObjects を照合して更新 or 追加
    // ----------------------------------------
    for (const auto& obj : objects)
    {
        XMLElement* element = nullptr;

        auto it = existingMap.find(obj.objectName);
        if (it != existingMap.end()) {
            // 既存オブジェクト → 更新
            element = it->second;
        }
        else {
            // 新規オブジェクト → 追加
            element = doc.NewElement("Object");
            element->SetAttribute("ID", obj.id);
            root->InsertEndChild(element);
        }

        // 新規エレメント追加関数
        auto AddValueElement = [&](const char* tag, const std::string& value)
            {
                XMLElement* e = doc.NewElement(tag);
                e->SetAttribute("Value", value.c_str());
                element->InsertEndChild(e);
            };

        // 属性設定（上書き）
        // 文字列属性
        // 新規オブジェクト → 追加
        AddValueElement("ObjectName", obj.objectName.c_str());
        AddValueElement("ObjectType", obj.objectType.c_str());
        AddValueElement("Tag", obj.objectTag.c_str());
        AddValueElement("FileName", obj.fileName.c_str());
        AddValueElement("Texture", obj.texture.c_str());

        // Animations（可変個）
        if (!obj.animations.empty())
        {
            XMLElement* animsElem = doc.NewElement("Animations");
            element->InsertEndChild(animsElem);

            for (const auto& anim : obj.animations)
            {
                XMLElement* animElem = doc.NewElement("Animation");
                animElem->SetAttribute("Name", anim.name.c_str());
                animElem->SetAttribute("Path", anim.path.c_str());
                animsElem->InsertEndChild(animElem);
            }
        }

        // float属性
        XMLElement* pos = doc.NewElement("Posision");
        pos->SetAttribute("X", obj.pos[0]);
        pos->SetAttribute("Y", obj.pos[1]);
        pos->SetAttribute("Z", obj.pos[2]);
        element->InsertEndChild(pos);

        XMLElement* rot = doc.NewElement("Rotation");
        rot->SetAttribute("X", obj.rot[0]);
        rot->SetAttribute("Y", obj.rot[1]);
        rot->SetAttribute("Z", obj.rot[2]);
        element->InsertEndChild(rot);

        XMLElement* scl = doc.NewElement("Scale");
        element->InsertEndChild(scl);
        scl->SetAttribute("X", obj.scl[0]);
        scl->SetAttribute("Y", obj.scl[1]);
        scl->SetAttribute("Z", obj.scl[2]);

        XMLElement* col = doc.NewElement("Color");        
        col->SetAttribute("R", obj.color[0]);
        col->SetAttribute("G", obj.color[1]);
        col->SetAttribute("B", obj.color[2]);
        col->SetAttribute("A", obj.color[3]);
        element->InsertEndChild(col);
    }

    // ----------------------------------------
    // 削除オプション（新しいデータに無い Object を削除）
    // ----------------------------------------
    bool enableDelete = true;

    if (enableDelete)
    {
        for (auto& it : existingMap)
        {
            bool existsInNew = false;
            for (auto& obj : objects)
            {
                if (obj.objectName == it.first) {
                    existsInNew = true;
                    break;
                }
            }

            if (!existsInNew) {
                // 削除
                root->DeleteChild(it.second);
            }
        }
    }

    // 保存
    XMLError err = doc.SaveFile(fileName.c_str());
    if (err != XML_SUCCESS) {
        std::cout << "Failed to save XML:" << fileName << std::endl;
    }
    else
    {
        std::cout << "Success to save XML:" << fileName << std::endl;
    }
}
