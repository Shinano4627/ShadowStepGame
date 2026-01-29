#pragma once

#include <string>
#include <unordered_map>
#include "Component.h"
#include "Game.h"

class GameObject;
class Texture2D;

//=========================================
// UIAnimationComponent.h
// UI用アニメーション制御
//=========================================
class UIAnimationComponent:public Component
{
private:
	//内部アニメ状態
	enum class AnimState
	{
		None,
		Playng,
	};

	struct AnimationClip
	{
		int startFrame = 0;
		int frameCount = 1;
		float frameTime = 0.1f;	//1フレームの秒数

		int sheetCol = 3;	//横分割
		int sheetRow = 3;	//縦分割
		bool loop = false;

		bool reverse = false;
	};

	GameObject* m_pOwner = nullptr;
	Texture2D* m_pTexture = nullptr;

	AnimState m_State = AnimState::None;
	float m_Timer = 0.0f;
	int m_CurrentFrame = 0;

	AnimationClip* m_pCurrentClip = nullptr;

	std::unordered_map<std::string, AnimationClip>m_Clips;

public:
	UIAnimationComponent();
	~UIAnimationComponent();

	void Init();
	void Update();
	void Uninit();

	//アニメーション再生
	void Play(const std::string& animName);

private:
	void UpdateAnimation();
	void ApplyFrameUV(int frameIndex);
};