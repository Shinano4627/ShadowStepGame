// ¦SceneBase.cppˆÈŠO‚ÉInclude‚µ‚È‚¢‚±‚ÆII
#pragma once
#include "SimpleCubeRendererComponent.h"
#include "SimplePlaneRendererComponent.h"
#include "MeshRendererComponent.h"

template <typename T>
struct TypeTag {};

TypeTag<SimpleCubeRendererComponent>    tag_cube;
TypeTag<SimplePlaneRendererComponent>   tag_plane;
TypeTag<MeshRendererComponent>   tag_model;

std::string strSimpleCube = "SimpleCube";
std::string strSimplePlane = "SimplePlane";
std::string strModel = "Model";

std::unordered_map<std::string, void*> typeTags = {
    { strSimpleCube, &tag_cube },
    { strSimplePlane, &tag_plane },
    { strModel,& tag_model },
};

// Œ^‚É‰‚¶‚½ˆ—
//template <typename T>
//T* Process(TypeTag<T>) {
//    // T ‚É‚æ‚éˆ—
//    return new T;
//}
