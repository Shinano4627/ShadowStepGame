#pragma once
#include <map>
#include <cassert>

#include "input.h"
#include "singleton.h"

enum INPUT_TYPE
{
	TYPE_OK,
	TYPE_CANCEL,

	TYPE_COUNT,
};

enum INPUT_MODE
{
	MODE_KEYBORD,
	MODE_CONTROLLER_XBOX,
	MODE_BOTH,	// コントローラとキーボード両方使用できる状態

	MODE_COUNT,
};

class IOManager
{
private:
	INPUT_MODE m_mode = MODE_KEYBORD;
	std::map<INPUT_MODE, std::map<INPUT_TYPE, int>> m_keyBindTable;
	
	Input m_Input;

public:

private:	
	int ChangeKeycode_SW(INPUT_TYPE _inpput);
	bool GetKeyDownController(INPUT_TYPE _key);
	bool GetKeyPressController(INPUT_TYPE _key);
	bool GetKeyUpController(INPUT_TYPE _key);

public:
	IOManager();
	void Init();
	void UnInit();
	void Update();
	void SetInputMode(INPUT_MODE);
	INPUT_MODE GetInputMode();

	bool GetKeyDown(INPUT_TYPE);		// キー押下
	bool GetKeyPress(INPUT_TYPE);		// キー長押し
	bool GetKeyUp(INPUT_TYPE);			// キーが離された

	bool GetKeyDownKeyBord(int _key);
	bool GetKeyPressKeyBord(int _key);
	bool GetKeyUpKeyBord(int _key);
};

#define IO_MANAGER Singleton<IOManager>::GetInstance()
