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

        // 文字列属性
        data.objectName = elem->Attribute("ObjectName");
        data.objectType = elem->Attribute("ObjectType");
        data.objectTag = elem->Attribute("Tag");
        data.fileName = elem->Attribute("FileName");
        data.texture = elem->Attribute("Texture");

        // float属性
        data.pos[0] = GetFloatAttribute(elem, "PosX");
        data.pos[1] = GetFloatAttribute(elem, "PosY");
        data.pos[2] = GetFloatAttribute(elem, "PosZ");

        data.rot[0] = GetFloatAttribute(elem, "RotX");
        data.rot[1] = GetFloatAttribute(elem, "RotY");
        data.rot[2] = GetFloatAttribute(elem, "RotZ");

        data.scl[0] = GetFloatAttribute(elem, "SclX", 1.0f);
        data.scl[1] = GetFloatAttribute(elem, "SclY", 1.0f);
        data.scl[2] = GetFloatAttribute(elem, "SclZ", 1.0f);

        data.color[0] = GetFloatAttribute(elem, "ColR", 1.0f);
        data.color[1] = GetFloatAttribute(elem, "ColG", 1.0f);
        data.color[2] = GetFloatAttribute(elem, "ColB", 1.0f);
        data.color[3] = GetFloatAttribute(elem, "ColA", 1.0f);

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
    // 1. newObjects を照合して更新 or 追加
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
            root->InsertEndChild(element);
        }

        // 属性設定（上書き）
        // 文字列属性
        element->SetAttribute("ObjectName", obj.objectName.c_str());
        element->SetAttribute("ObjectType", obj.objectType.c_str());
        element->SetAttribute("Tag", obj.objectTag.c_str());
        element->SetAttribute("FileName", obj.fileName.c_str());
        element->SetAttribute("Texture", obj.texture.c_str());

        // float属性
        element->SetAttribute("PosX", obj.pos[0]);
        element->SetAttribute("PosY", obj.pos[1]);
        element->SetAttribute("PosZ", obj.pos[2]);

        element->SetAttribute("RotX", obj.rot[0]);
        element->SetAttribute("RotY", obj.rot[1]);
        element->SetAttribute("RotZ", obj.rot[2]);

        element->SetAttribute("SclX", obj.scl[0]);
        element->SetAttribute("SclY", obj.scl[1]);
        element->SetAttribute("SclZ", obj.scl[2]);

        element->SetAttribute("ColR", obj.color[0]);
        element->SetAttribute("ColG", obj.color[1]);
        element->SetAttribute("ColB", obj.color[2]);
        element->SetAttribute("ColA", obj.color[3]);
    }

    // ----------------------------------------
    // 2. 削除オプション（新しいデータに無い Object を削除）
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
        printf("Failed to save XML: %s\n", fileName);
    }
}
