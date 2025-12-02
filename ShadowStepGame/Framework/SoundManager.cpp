#include "SoundManager.h"

Sound sound;

void SoundManager::Init()
{
	sound.Init();
}
void SoundManager::UnInit()
{
	sound.Uninit();
}

void SoundManager::Update()
{
	
}

void SoundManager::PlayBGM(SOUND_LABEL _sound)
{
	sound.Play(_sound);
	sound.SetVolume(_sound, m_volumeBGM);
	m_currentBGM = _sound;
}

void SoundManager::PlaySE(SOUND_LABEL _sound)
{
	sound.Play(_sound);
	sound.SetVolume(_sound, m_volumeSE);
}

void SoundManager::Stop(SOUND_LABEL _sound)
{
	sound.Stop(_sound);
}

float SoundManager::GetVolumeBGM()
{
	return m_volumeBGM;
}
void SoundManager::SetVolumeBGM(float _data)
{
	m_volumeBGM = _data;
	sound.SetVolume(m_currentBGM, _data);
}
float SoundManager::GetVolumeSE()
{
	return m_volumeSE;
}
void SoundManager::SetVolumeSE(float _data)
{
	m_volumeSE = _data;
}