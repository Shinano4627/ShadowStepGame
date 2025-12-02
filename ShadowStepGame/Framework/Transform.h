// ===================================================================
// Transform.h
// GameObjectの姿勢情報を管理するクラス
// ===================================================================
#pragma once
#include <SimpleMath.h>
#include "SystemCommon.h"

// ===================================================================
// Transform クラス
// GameObjectが所持する姿勢情報クラス
// ===================================================================
class Transform
{
private:
    // ===================================================================
    // メンバ変数
    // ===================================================================

    // ワールド座標(x,y,z)
    Vector3 m_Position = Vector3(0.0f, 0.0f, 0.0f);
    // 回転角度(Euler角 (Pitch, Yaw, Roll))
    Vector3 m_Rotation = Vector3(0.0f, 0.0f, 0.0f);
    // スケール(z,y,z)
    Vector3 m_Scale = Vector3(1.0f, 1.0f, 1.0f);

    // ワールド座標(x,y,z) 初期値
    Vector3 m_PositionInit = Vector3(0.0f, 0.0f, 0.0f);
    // 回転角度(Euler角 (Pitch, Yaw, Roll))　初期値
    Vector3 m_RotationInit = Vector3(0.0f, 0.0f, 0.0f);
    // スケール(z,y,z)　初期値
    Vector3 m_ScaleInit = Vector3(1.0f, 1.0f, 1.0f);

public:
    // ===================================================================
    // コンストラクタ
    // ===================================================================
    Transform()
        : m_Position(0.0f, 0.0f, 0.0f)
        , m_Rotation(0.0f, 0.0f, 0.0f)
        , m_Scale(1.0f, 1.0f, 1.0f){}
    Transform(Vector3 pos, Vector3 rotate, Vector3 scale)
        : m_PositionInit(pos)
        , m_RotationInit(rotate)
        , m_ScaleInit(scale)
    {
        m_Position = m_PositionInit;
        m_Rotation = m_RotationInit;
        m_Scale = m_ScaleInit;
    }

    // ===================================================================
    // Set関数
    // ===================================================================
    void SetPosition(const Vector3& pos) { m_Position = pos; }
    void SetRotation(const Vector3& rot) { m_Rotation = rot; }
    void SetScale(const Vector3& scale) { m_Scale = scale; }

    // ===================================================================
    // Get関数
    // ===================================================================
    const Vector3& GetPosition() const { return m_Position; }
    const Vector3& GetRotation() const { return m_Rotation; }
    const Vector3& GetScale() const { return m_Scale; }
    void ConvertData(float* pos, float* rot, float* scale)
    {
        pos[0] = m_Position.x;
        pos[1] = m_Position.y;
        pos[2] = m_Position.z;

        rot[0] = m_Rotation.x;
        rot[1] = m_Rotation.y;
        rot[2] = m_Rotation.z;

        scale[0] = m_Scale.x;
        scale[1] = m_Scale.y;
        scale[2] = m_Scale.z;
    }

    // ===================================================================
    // ワールド行列の計算
    // ===================================================================
    Matrix GetWorldMatrix() const
    {
        // Scale行列（拡大縮小変換）
        Matrix s = Matrix::CreateScale(m_Scale);
        // Rotation行列（回転変換）
        Matrix r = Matrix::CreateFromYawPitchRoll(
            m_Rotation.y,  // Yaw   (Y軸回転)
            m_Rotation.x,  // Pitch (X軸回転)
            m_Rotation.z   // Roll  (Z軸回転)
        );

        // Translation行列（平行移動変換）
        Matrix t = Matrix::CreateTranslation(m_Position);

        // 合成: S * R * T
        return s * r * t;
    }

    // ===================================================================
    // 相対移動・回転・スケール
    // ===================================================================

    // 指定した量だけ移動
    void Translate(const Vector3& delta)
    {
        m_Position += delta;
    }

    // 指定した量だけ回転
    void Rotate(const Vector3& delta)
    {
        m_Rotation += delta;
    }

    // 指定した量だけスケール変更
    void Scale(const Vector3& delta)
    {
        m_Scale += delta;
    }

    // ===================================================================
    // ローカル軸ベクトルの取得
    // ===================================================================

    // 前方ベクトル (Z軸正方向)
    Vector3 GetForward() const
    {
        Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
        return Vector3(r._31, r._32, r._33);
    }

    // 上ベクトル (Y軸正方向)
    Vector3 GetUp() const
    {
        Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
        return Vector3(r._21, r._22, r._23);
    }

    // 右ベクトル (X軸正方向)
    Vector3 GetRight() const
    {
        Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
        return Vector3(r._11, r._12, r._13);
    }

    // ===================================================================
    // リセット
    // ===================================================================
    void Reset()
    {
        m_Position = m_PositionInit;
        m_Rotation = m_RotationInit;
        m_Scale = m_ScaleInit;
    }
};