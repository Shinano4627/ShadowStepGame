// ¦SceneBase.cppˆÈŠO‚ÉInclude‚µ‚È‚¢‚±‚ÆII
#pragma once
#include "SimpleCubeRendererComponent.h"
#include "SimplePlaneRendererComponent.h"
#include "MeshRendererComponent.h"
#include "Texture2D.h"

template <typename T>
struct TypeTag {};

TypeTag<SimpleCubeRendererComponent>    tag_cube;
TypeTag<SimplePlaneRendererComponent>   tag_plane;
TypeTag<MeshRendererComponent>   tag_model;
TypeTag<Texture2D>   tag_2D;

std::string strSimpleCube = "SimpleCube";
std::string strSimplePlane = "SimplePlane";
std::string strModel = "Model";
std::string str2D = "2D";

std::unordered_map<std::string, void*> typeTags = {
    { strSimpleCube, &tag_cube },
    { strSimplePlane, &tag_plane },
    { strModel,& tag_model },
    { str2D,&tag_2D },
};

// Œ^‚É‰‚¶‚½ˆ—
//template <typename T>
//T* Process(TypeTag<T>) {
//    // T ‚É‚æ‚éˆ—
//    return new T;
//}
