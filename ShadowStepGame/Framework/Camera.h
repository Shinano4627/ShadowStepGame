#pragma once

#include	<SimpleMath.h>

//-----------------------------------------------------------------------------
//Cameraクラス
//-----------------------------------------------------------------------------
class Camera {
private:
	DirectX::SimpleMath::Vector3	m_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3	m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3	m_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

	DirectX::SimpleMath::Vector3	m_Target{};
	DirectX::SimpleMath::Matrix		m_ViewMatrix{};

public:
	void Init();
	void Update();
	void SetCamera(int mode = 0);
	void Uninit();

	// ================================================================== =
	// アクセサ
	// ===================================================================

	// 位置の設定・取得
	void SetPosition(const DirectX::SimpleMath::Vector3 & position) { m_Position = position; }
	const DirectX::SimpleMath::Vector3& GetPosition() const { return m_Position; }

	// ターゲット（注視点）の設定・取得
	void SetTarget(const DirectX::SimpleMath::Vector3& target) { m_Target = target; }
	const DirectX::SimpleMath::Vector3& GetTarget() const { return m_Target; }

	// 回転の設定・取得
	void SetRotation(const DirectX::SimpleMath::Vector3& rotation) { m_Rotation = rotation; }
	const DirectX::SimpleMath::Vector3& GetRotation() const { return m_Rotation; }

};