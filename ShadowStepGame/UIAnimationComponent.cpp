#include "UIAnimationComponent.h"
#include "GameObject.h"
#include "Texture2D.h"


UIAnimationComponent::UIAnimationComponent()
{
}

UIAnimationComponent::~UIAnimationComponent()
{

}

//=========================================
//初期化
//=========================================
void UIAnimationComponent::Init()
{

	m_pOwner = GetOwner();
	if (!m_pOwner)return;

	m_pTexture = m_pOwner->GetMeshComponent<Texture2D>();

	Play("Select");

	//選択アニメ(0~6の7コマ)
	{
		AnimationClip select;
		select.startFrame = 0;
		select.frameCount = 7;
		select.frameTime = 0.08f;
		select.sheetCol = 7;
		select.sheetRow = 1;
		select.loop = false;
		select.reverse = false;

		m_Clips["Select"] = select;
	}

	{
		AnimationClip deselect;
		deselect.startFrame = 0;
		deselect.frameCount = 7;
		deselect.frameTime = 0.08f;
		deselect.sheetCol = 7;
		deselect.sheetRow = 1;
		deselect.loop = false;
		deselect.reverse = true;

		m_Clips["Deselect"] = deselect;
	}
}

//=========================================
//更新
//=========================================
void UIAnimationComponent::Update()
{

	if (m_State == AnimState::Playng)
	{
		UpdateAnimation();
	}
}

//=========================================
//終了
//=========================================
void UIAnimationComponent::Uninit()
{
	m_State = AnimState::None;
	m_pCurrentClip = nullptr;
}

//=========================================
//再生
//=========================================
void UIAnimationComponent::Play(const std::string& animName)
{
	auto it = m_Clips.find(animName);
	if (it == m_Clips.end())
		return;

	m_pCurrentClip = &it->second;
	m_State = AnimState::Playng;
	m_Timer = 0.0f;
	
	//逆再生なら最後から、通常なら最初から
	if (m_pCurrentClip->reverse)
	{
		m_CurrentFrame = m_pCurrentClip->frameCount - 1;
	}
	else
	{
		m_CurrentFrame = 0;
	}

	//最初のフレームを即反映
	ApplyFrameUV(m_CurrentFrame);
}

//=========================================
//アニメ更新
//=========================================
void UIAnimationComponent::UpdateAnimation()
{
	if (!m_pCurrentClip || !m_pTexture)
		return;

	m_Timer += Game::GetDeltaTime();

	if (m_Timer >= m_pCurrentClip->frameTime)
		return;

	m_Timer -= m_pCurrentClip->frameTime;

	//再生方向
	if(m_pCurrentClip->reverse)
	{
		m_CurrentFrame--;
		if (m_CurrentFrame < 0)
		{
			m_State = AnimState::None;
			return;
		}
	}
	else
	{
		m_CurrentFrame++;
		if (m_CurrentFrame >= m_pCurrentClip->frameCount)
		{
			if (m_pCurrentClip->loop)
				m_CurrentFrame = 0;
			else
			{
				m_State = AnimState::None;
				return;
			}
		}
	}
	ApplyFrameUV(m_CurrentFrame);
}

//=========================================
//UV適用(仮)
//=========================================
void UIAnimationComponent::ApplyFrameUV(int frameIndex)
{
	if (!m_pCurrentClip || !m_pTexture)
		return;

	//実際のフレーム番号(シート基準)
	int sheetFrame = m_pCurrentClip->startFrame + frameIndex;

	int col = m_pCurrentClip->sheetCol;
	int row = m_pCurrentClip->sheetRow;

	//frame->(u,v)変換
	int u = sheetFrame % col;
	int v = sheetFrame / col;

	m_pTexture->SetUV(u+1,v+1,col,row);


}