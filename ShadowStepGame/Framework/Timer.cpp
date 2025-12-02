// ===================================================================
// Timer.cpp
// ===================================================================

#include "Timer.h"
#include <algorithm>

// ===================================================================
// コンストラクタ
// ===================================================================
Timer::Timer()
	: m_LastTime(Clock::now())
	, m_DeltaTime(0.0f)
	, m_TotalTime(0.0f)
	, m_FrameCount(0)
	, m_FpsTimer(0.0f)
	, m_FpsFrameCount(0)
	, m_CurrentFps(0.0f)
{
}

// ===================================================================
// 更新処理
// ===================================================================
void Timer::Update()
{
	// 現在時刻を取得
	TimePoint currentTime = Clock::now();

	// 前フレームからの経過時間を計算
	Duration elapsed = currentTime - m_LastTime;
	m_DeltaTime = elapsed.count();

	// スパイク対策：異常に長いデルタタイムを制限
	if (m_DeltaTime > MAX_DELTA_TIME)
	{
		m_DeltaTime = MAX_DELTA_TIME;
	}

	// 総経過時間を更新
	m_TotalTime += m_DeltaTime;

	// フレーム数をカウント
	m_FrameCount++;

	// FPS計測
	m_FpsTimer += m_DeltaTime;
	m_FpsFrameCount++;

	// 1秒経過したらFPSを更新
	if (m_FpsTimer >= 1.0f)
	{
		// FPS = フレーム数 / 経過時間
		m_CurrentFps = static_cast<float>(m_FpsFrameCount) / m_FpsTimer;

		// カウンターをリセット
		m_FpsTimer = 0.0f;
		m_FpsFrameCount = 0;
	}

	// 次フレームのために現在時刻を保存
	m_LastTime = currentTime;
}


// ===================================================================
// リセット
// ===================================================================
void Timer::Reset()
{
	m_LastTime = Clock::now();
	m_DeltaTime = 0.0f;
	m_TotalTime = 0.0f;
	m_FrameCount = 0;
	m_FpsTimer = 0.0f;
	m_FpsFrameCount = 0;
	m_CurrentFps = 0.0f;
}