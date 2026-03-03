#include "IOManager.h"
#include "Application.h"

//#include "controllerSW.h"

IOManager::IOManager()
{
	//Init();
}

void IOManager::Init()
{
	m_keyBindTable[MODE_KEYBORD] =
	{
		{TYPE_OK, VK_RETURN},
		{TYPE_CANCEL, VK_CANCEL},
	};

	// マウスをウインドウにバインド
	m_mouse.SetWindow(Application::GetWindow());
	// 絶対座標を取得する
	SetMouseMode(DirectX::Mouse::Mode::MODE_ABSOLUTE);

	//InitController();
}
void IOManager::UnInit()
{
	
	//UninitController();
}

void IOManager::Update()
{
	m_Input.Update();

	// マウスの状態を取得
	auto mouseState = m_mouse.GetState();
	// マウスの移動量を取得
	m_mouseDeltaX = mouseState.x;
	m_mouseDeltaY = mouseState.y;
	//UpdateController();
}

void IOManager::SetInputMode(INPUT_MODE _mode)
{
	m_mode = _mode;
}

INPUT_MODE IOManager::GetInputMode()
{
	return m_mode;
}

bool IOManager::GetKeyDown(INPUT_TYPE _key)
{
	bool ret = false;

	assert(_key < TYPE_COUNT);		// エラーチェック

	switch (m_mode)
	{
	case MODE_KEYBORD:
		ret = GetKeyDownKeyBord(m_keyBindTable[MODE_KEYBORD][_key]);
		break;
	case MODE_CONTROLLER_XBOX:
		break;
	case MODE_BOTH:
		break;
	default:
		break;
	}
	
	return ret;
}
bool IOManager::GetKeyPress(INPUT_TYPE _key)
{
	bool ret = false;

	assert(_key < TYPE_COUNT);		// エラーチェック

	switch (m_mode)
	{
	case MODE_KEYBORD:
		ret = GetKeyPressKeyBord(m_keyBindTable[MODE_KEYBORD][_key]);
		break;
	case MODE_CONTROLLER_XBOX:
		break;
	case MODE_BOTH:
		break;
	default:
		break;
	}

	return ret;
}
bool IOManager::GetKeyUp(INPUT_TYPE _key)
{
	bool ret = false;

	assert(_key < TYPE_COUNT);		// エラーチェック

	switch (m_mode)
	{
	case MODE_KEYBORD:
		ret = GetKeyUpKeyBord(m_keyBindTable[MODE_KEYBORD][_key]);
		break;
	case MODE_CONTROLLER_XBOX:
		break;
	case MODE_BOTH:
		break;
	default:
		break;
	}

	return ret;
}


// キー押下
bool IOManager::GetKeyDownController(INPUT_TYPE _key)
{
	bool ret = false;

#ifndef _WIN64
	ret = GetControllerTrigger(ChangeKeycode_SW(_key));
#endif //_WIN64
	return ret;
}

// キー長押し検知
bool IOManager::GetKeyPressController(INPUT_TYPE _key)
{
	bool ret = false;

#ifndef _WIN64
	ret = GetControllerPress(ChangeKeycode_SW(_key));
#endif //_WIN64
	return ret;
}

// キーが離された
bool IOManager::GetKeyUpController(INPUT_TYPE _key)
{
	bool ret = false;

#ifndef _WIN64
	ret = GetControllerRelease(ChangeKeycode_SW(_key));
#endif //_WIN64
	return ret;
}

bool IOManager::GetKeyDownKeyBord(int _key)
{
	return m_Input.GetKeyTrigger(_key);
}
bool IOManager::GetKeyPressKeyBord(int _key)
{
	return m_Input.GetKeyPress(_key);
}
bool IOManager::GetKeyUpKeyBord(int _key)
{
	return m_Input.GetKeyRelease(_key);
}

int IOManager::ChangeKeycode_SW(INPUT_TYPE _inpput)
{
	int ret = -1;
#ifndef _WIN64
	switch (_inpput)
	{
	case INPUT_TYPE_A:
		ret = NpadButton::A::Index;
		break;
	case INPUT_TYPE_B:
		ret = NpadButton::B::Index;
		break;
	case INPUT_TYPE_X:
		ret = NpadButton::X::Index;
		break;
	case INPUT_TYPE_Y:
		ret = NpadButton::Y::Index;
		break;
	case INPUT_TYPE_UP:
		ret = NpadButton::Up::Index;
		break;
	case INPUT_TYPE_DOWN:
		ret = NpadButton::Down::Index;
		break;
	case INPUT_TYPE_RIGHT:
		ret = NpadButton::Right::Index;
		break;
	case INPUT_TYPE_LEFT:
		ret = NpadButton::Left::Index;
		break;
	case INPUT_TYPE_RS:
		break;
	case INPUT_TYPE_LS:
		break;
	case INPUT_TYPE_R:
		break;
	case INPUT_TYPE_ZR:
		break;
	case INPUT_TYPE_L:
		break;
	case INPUT_TYPE_ZL:
		break;
	case INPUT_TYPE_PLUS:
		break;
	case INPUT_TYPE_MINUS:
		break;
	case INPUT_TYPE_NUM:
		break;
		// スティックの判定は要検討
	case INPUT_TYPE_RS_UP:
		break;
	case INPUT_TYPE_RS_DOWN:
		break;
	case INPUT_TYPE_RS_RIGHT:

		break;
	case INPUT_TYPE_RS_LEFT:
		break;
	case INPUT_TYPE_LS_UP:
		break;
	case INPUT_TYPE_LS_DOWN:
		break;
	case INPUT_TYPE_LS_RIGHT:
		break;
	case INPUT_TYPE_LS_LEFT:
		break;
	default:
		break;
	}

#endif //_WIN64
	return ret;
}

//Vector2 IOManager::GetLeftStickInclination()
//{
//	Vector2 ret = {0, 0};
//#ifndef _WIN64
//	Float2 data = GetControllerLeftStick();
//	ret.x = data.x;
//	ret.y = data.y;
//#endif //_WIN64
//	return ret;
//}
//Vector2 IOManager::GetRightStickInclination()
//{
//	Vector2 ret = { 0, 0 };
//#ifndef _WIN64
//	Float2 data = ::GetControllerRightStick();
//	ret.x = data.x;
//	ret.y = data.y;
//#endif //_WIN64
//	return ret;
//}