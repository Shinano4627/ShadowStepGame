#include "input.h"

// コンストラクタ
Input::Input()
	: VibrationTime(0)
{
	// 配列を初期化
	ZeroMemory(keyState, sizeof(keyState));
	ZeroMemory(keyState_old, sizeof(keyState_old));
	ZeroMemory(&controllerState, sizeof(controllerState));
	ZeroMemory(&controllerState_old, sizeof(controllerState_old));
}

// デストラクタ
Input::~Input()
{
	// 振動を終了させる
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = 0;
	vibration.wRightMotorSpeed = 0;
	XInputSetState(0, &vibration);
}

void Input::Update()
{
	// 1フレーム前の入力を記録しておく
	for (int i = 0; i < 256; i++)
	{
		keyState_old[i] = keyState[i];
	}
	controllerState_old = controllerState;

	// キー入力を更新
	BOOL hr = GetKeyboardState(keyState);

	// コントローラー入力を更新(XInput)
	XInputGetState(0, &controllerState);

	// 振動継続時間をカウント
	if (VibrationTime > 0)
	{
		VibrationTime--;
		if (VibrationTime == 0)
		{
			// 振動継続時間が経った時に振動を止める
			XINPUT_VIBRATION vibration;
			ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
			vibration.wLeftMotorSpeed = 0;
			vibration.wRightMotorSpeed = 0;
			XInputSetState(0, &vibration);
		}
	}
}

// キー入力
bool Input::GetKeyPress(int key)
{
	return keyState[key] & 0x80;
}

bool Input::GetKeyTrigger(int key)
{
	return (keyState[key] & 0x80) && !(keyState_old[key] & 0x80);
}

bool Input::GetKeyRelease(int key)
{
	return !(keyState[key] & 0x80) && (keyState_old[key] & 0x80);
}

// 左アナログスティック
DirectX::XMFLOAT2 Input::GetLeftAnalogStick(void)
{
	SHORT x = controllerState.Gamepad.sThumbLX;
	SHORT y = controllerState.Gamepad.sThumbLY;

	DirectX::XMFLOAT2 res;
	res.x = x / 32767.0f;
	res.y = y / 32767.0f;
	return res;
}

// 右アナログスティック
DirectX::XMFLOAT2 Input::GetRightAnalogStick(void)
{
	SHORT x = controllerState.Gamepad.sThumbRX;
	SHORT y = controllerState.Gamepad.sThumbRY;

	DirectX::XMFLOAT2 res;
	res.x = x / 32767.0f;
	res.y = y / 32767.0f;
	return res;
}

// 左トリガー
float Input::GetLeftTrigger(void)
{
	BYTE t = controllerState.Gamepad.bLeftTrigger;
	return t / 255.0f;
}

// 右トリガー
float Input::GetRightTrigger(void)
{
	BYTE t = controllerState.Gamepad.bRightTrigger;
	return t / 255.0f;
}

// ボタン入力
bool Input::GetButtonPress(WORD btn)
{
	return (controllerState.Gamepad.wButtons & btn) != 0;
}

bool Input::GetButtonTrigger(WORD btn)
{
	return (controllerState.Gamepad.wButtons & btn) != 0 &&
		(controllerState_old.Gamepad.wButtons & btn) == 0;
}

bool Input::GetButtonRelease(WORD btn)
{
	return (controllerState.Gamepad.wButtons & btn) == 0 &&
		(controllerState_old.Gamepad.wButtons & btn) != 0;
}

// 振動
void Input::SetVibration(int frame, float power)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	vibration.wLeftMotorSpeed = (WORD)(power * 65535.0f);
	vibration.wRightMotorSpeed = (WORD)(power * 65535.0f);
	XInputSetState(0, &vibration);

	VibrationTime = frame;
}