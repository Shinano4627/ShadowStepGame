//=======================================
// XmlRW.h
// xmlファイルを読み込み各SceneのGameObjectDataを作成する
//=======================================
#pragma once
#include <vector>
#include <string>

struct AnimationData
{
    std::string name;
    std::string path;
};

struct ObjectData
{
    int id;
    std::string objectName;
    std::string objectType;
    std::string objectTag;
    std::string fileName;
    std::string texture;
    std::vector<AnimationData> animations;

    float pos[3];
    float rot[3];
    float scl[3];
    float color[4];
};

class XmlRW
{
private:

public:
    int GetObjectData(const char* _stage, std::vector<ObjectData>& _objectList);
    void UpdateObjectsInXML(const std::vector<ObjectData>& objects, const char* _stage);
};

