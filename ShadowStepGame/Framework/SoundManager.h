#pragma once
#include "Application.h"
#include "sound.h"
#include "singleton.h"

class SoundManager
{
/// <summary>
/// 変数
/// </summary>
private:
	float m_volumeBGM = 1.0f;
	float m_volumeSE = 1.0f;

	SOUND_LABEL m_currentBGM = SOUND_NONE;
public:

/// <summary>
/// 関数
/// </summary>
private:

public:
	void Init();
	void UnInit();
	void Update();
	void PlayBGM(SOUND_LABEL _sound);
	void PlaySE(SOUND_LABEL _sound);
	void Stop(SOUND_LABEL _sound);

	float GetVolumeBGM();
	void SetVolumeBGM(float _data);
	float GetVolumeSE();
	void SetVolumeSE(float _data);
};

// 唯一インスタンス取得マクロ
#define SOUND_MANAGER Singleton<SoundManager>::GetInstance()