////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;


	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_mouseX = screenWidth/2;
	m_mouseY = screenHeight/2;
	m_mouseZ = 100;

	m_mouseDeltaX = 0.0f;
	m_mouseDeltaY = 0.0f;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}
	
	// Acquire the mouse.
	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

void InputClass::Shutdown()
{
	// Release the mouse.
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

bool InputClass::Frame()
{
	bool result;


	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;


	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
		
	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;
	m_mouseZ += m_mouseState.lZ/60;

	m_mouseDeltaX = m_mouseState.lX;
	m_mouseDeltaY = m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if(m_mouseX < 0)  { m_mouseX = 0; }
	if(m_mouseY < 0)  { m_mouseY = 0; }
	
	if(m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
	
	return;
}

bool InputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if(m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY, int& mouseZ, float& mouseDeltaX, float& mouseDeltaY)
{
	mouseX = m_mouseX+50;
	mouseY = m_mouseY;
	mouseZ = m_mouseZ;
	mouseDeltaX = m_mouseDeltaX;
	mouseDeltaY = m_mouseDeltaY;
	return;
}

void InputClass::IsKeyPressed (bool* keys){

	if(m_keyboardState[DIK_W] & 0x80)
	{
		keys[0] = true;

	} else keys[0] = false;

	if(m_keyboardState[DIK_A] & 0x80)
	{
		keys[1] = true;

	} else keys[1] = false;

	if(m_keyboardState[DIK_S] & 0x80)
	{
		keys[2] = true;

	} else keys[2] = false;

	if(m_keyboardState[DIK_D] & 0x80)
	{
		keys[3] = true;

	} else keys[3] = false;

	if(m_keyboardState[DIK_Q] & 0x80)
	{
		keys[4] = true;

	} else keys[4] = false;

	if(m_keyboardState[DIK_E] & 0x80)
	{
		keys[5] = true;

	} else keys[5] = false;

	if(m_keyboardState[DIK_R] & 0x80)
	{
		keys[6] = true;

	} else keys[6] = false;

	if(m_keyboardState[DIK_G] & 0x80)
	{
		keys[7] = true;

	} else keys[7] = false;

	if(m_keyboardState[DIK_B] & 0x80)
	{
		keys[8] = true;

	} else keys[8] = false;

	if(m_keyboardState[DIK_0] & 0x80)
	{
		keys[9] = true;

	} else keys[9] = false;

	if(m_keyboardState[DIK_1] & 0x80)
	{
		keys[10] = true;

	} else keys[10] = false;

	if(m_keyboardState[DIK_2] & 0x80)
	{
		keys[11] = true;

	} else keys[11] = false;

	if(m_keyboardState[DIK_3] & 0x80)
	{
		keys[12] = true;

	} else keys[12] = false;

	if(m_keyboardState[DIK_4] & 0x80)
	{
		keys[13] = true;

	} else keys[13] = false;

	if(m_keyboardState[DIK_5] & 0x80)
	{
		keys[14] = true;

	} else keys[14] = false;

	if(m_keyboardState[DIK_6] & 0x80)
	{
		keys[15] = true;

	} else keys[15] = false;

	if(m_keyboardState[DIK_7] & 0x80)
	{
		keys[16] = true;

	} else keys[16] = false;

	if(m_keyboardState[DIK_8] & 0x80)
	{
		keys[17] = true;

	} else keys[17] = false;

	if(m_keyboardState[DIK_9] & 0x80)
	{
		keys[18] = true;

	} else keys[18] = false;

	if(m_keyboardState[DIK_LSHIFT] & 0x80)
	{
		keys[19] = true;

	} else keys[19] = false;

	if(m_keyboardState[DIK_DELETE] & 0x80)
	{
		keys[20] = true;

	} else keys[20] = false;

	if(m_keyboardState[DIK_LCONTROL] & 0x80)
	{
		keys[21] = true;

	} else keys[21] = false;

	return;
}

void InputClass::IsKeyPressed (KEYBOARD_KEYS* keys){

	if(m_keyboardState[DIK_A] & 0x80)
	{
		keys->key_A = true;

	} else keys->key_A = false;

	if(m_keyboardState[DIK_B] & 0x80)
	{
		keys->key_B = true;

	} else keys->key_B = false;

	if(m_keyboardState[DIK_C] & 0x80)
	{
		keys->key_C = true;

	} else keys->key_C = false;

	if(m_keyboardState[DIK_D] & 0x80)
	{
		keys->key_D = true;

	} else keys->key_D = false;

	if(m_keyboardState[DIK_E] & 0x80)
	{
		keys->key_E = true;

	} else keys->key_E = false;

	if(m_keyboardState[DIK_F] & 0x80)
	{
		keys->key_F = true;

	} else keys->key_F = false;

	if(m_keyboardState[DIK_G] & 0x80)
	{
		keys->key_G = true;

	} else keys->key_G = false;

	if(m_keyboardState[DIK_H] & 0x80)
	{
		keys->key_H = true;

	} else keys->key_H = false;

	if(m_keyboardState[DIK_I] & 0x80)
	{
		keys->key_I = true;

	} else keys->key_I = false;

	if(m_keyboardState[DIK_J] & 0x80)
	{
		keys->key_J = true;

	} else keys->key_J = false;

	if(m_keyboardState[DIK_K] & 0x80)
	{
		keys->key_K = true;

	} else keys->key_K = false;

	if(m_keyboardState[DIK_L] & 0x80)
	{
		keys->key_L = true;

	} else keys->key_L = false;

	if(m_keyboardState[DIK_M] & 0x80)
	{
		keys->key_M = true;

	} else keys->key_M = false;

	if(m_keyboardState[DIK_N] & 0x80)
	{
		keys->key_N = true;

	} else keys->key_N = false;

	if(m_keyboardState[DIK_O] & 0x80)
	{
		keys->key_O = true;

	} else keys->key_O = false;

	if(m_keyboardState[DIK_P] & 0x80)
	{
		keys->key_P = true;

	} else keys->key_P = false;

	if(m_keyboardState[DIK_Q] & 0x80)
	{
		keys->key_Q = true;

	} else keys->key_Q = false;

	if(m_keyboardState[DIK_R] & 0x80)
	{
		keys->key_R = true;

	} else keys->key_R = false;

	if(m_keyboardState[DIK_S] & 0x80)
	{
		keys->key_S = true;

	} else keys->key_S = false;

	if(m_keyboardState[DIK_T] & 0x80)
	{
		keys->key_T = true;

	} else keys->key_T = false;

	if(m_keyboardState[DIK_U] & 0x80)
	{
		keys->key_U = true;

	} else keys->key_U = false;

	if(m_keyboardState[DIK_V] & 0x80)
	{
		keys->key_V = true;

	} else keys->key_V = false;

	if(m_keyboardState[DIK_W] & 0x80)
	{
		keys->key_W = true;

	} else keys->key_W = false;

	if(m_keyboardState[DIK_X] & 0x80)
	{
		keys->key_X = true;

	} else keys->key_X = false;

	if(m_keyboardState[DIK_Y] & 0x80)
	{
		keys->key_Y = true;

	} else keys->key_Y = false;

	if(m_keyboardState[DIK_Z] & 0x80)
	{
		keys->key_Z = true;

	} else keys->key_Z = false;

	// for qwertz based layout
	if(/*m_keyboardState[DIK_0] & 0x80 ||*/ m_keyboardState[DIK_NUMPAD0] & 0x80 || m_keyboardState[DIK_GRAVE] & 0x80)
	{
		keys->key_0 = true;

	} else keys->key_0 = false;

	if(m_keyboardState[DIK_1] & 0x80 || m_keyboardState[DIK_NUMPAD1] & 0x80)
	{
		keys->key_1 = true;

	} else keys->key_1 = false;

	if(m_keyboardState[DIK_2] & 0x80 || m_keyboardState[DIK_NUMPAD2] & 0x80)
	{
		keys->key_2 = true;

	} else keys->key_2 = false;

	if(m_keyboardState[DIK_3] & 0x80 || m_keyboardState[DIK_NUMPAD3] & 0x80)
	{
		keys->key_3 = true;

	} else keys->key_3 = false;

	if(m_keyboardState[DIK_4] & 0x80 || m_keyboardState[DIK_NUMPAD4] & 0x80)
	{
		keys->key_4 = true;

	} else keys->key_4 = false;

	if(m_keyboardState[DIK_5] & 0x80 || m_keyboardState[DIK_NUMPAD5] & 0x80)
	{
		keys->key_5 = true;

	} else keys->key_5 = false;

	if(m_keyboardState[DIK_6] & 0x80 || m_keyboardState[DIK_NUMPAD6] & 0x80)
	{
		keys->key_6 = true;

	} else keys->key_6 = false;

	if(m_keyboardState[DIK_7] & 0x80 || m_keyboardState[DIK_NUMPAD7] & 0x80)
	{
		keys->key_7 = true;

	} else keys->key_7 = false;

	if(m_keyboardState[DIK_8] & 0x80 || m_keyboardState[DIK_NUMPAD8] & 0x80)
	{
		keys->key_8 = true;

	} else keys->key_8 = false;

	if(m_keyboardState[DIK_9] & 0x80 || m_keyboardState[DIK_NUMPAD9] & 0x80)
	{
		keys->key_9 = true;

	} else keys->key_9 = false;

	if(m_keyboardState[DIK_SPACE] & 0x80)
	{
		keys->key_SPACE = true;

	} else keys->key_SPACE = false;

	if(m_keyboardState[DIK_LCONTROL] & 0x80)
	{
		keys->key_LCONTROL = true;

	} else keys->key_LCONTROL = false;

	if(m_keyboardState[DIK_LSHIFT] & 0x80 || m_keyboardState[DIK_RSHIFT] & 0x80)
	{
		keys->key_SHIFT = true;

	} else keys->key_SHIFT = false;

	if(m_keyboardState[DIK_RETURN] & 0x80 || m_keyboardState[DIK_NUMPADENTER] & 0x80)
	{
		keys->key_RETURN = true;

	} else keys->key_RETURN = false;

	if(m_keyboardState[DIK_DELETE] & 0x80)
	{
		keys->key_DELETE = true;

	} else keys->key_DELETE = false;

	if(m_keyboardState[DIK_TAB] & 0x80)
	{
		keys->key_TAB = true;

	} else keys->key_TAB = false;

	if(m_keyboardState[DIK_BACK] & 0x80)
	{
		keys->key_BACK = true;

	} else keys->key_BACK = false;

	if(m_keyboardState[DIK_LEFT] & 0x80)
	{
		keys->key_LEFT = true;

	} else keys->key_LEFT = false;

	if(m_keyboardState[DIK_RIGHT] & 0x80)
	{
		keys->key_RIGHT = true;

	} else keys->key_RIGHT = false;

	if(m_keyboardState[DIK_UP] & 0x80)
	{
		keys->key_UP = true;

	} else keys->key_UP = false;

	if(m_keyboardState[DIK_DOWN] & 0x80)
	{
		keys->key_DOWN = true;

	} else keys->key_DOWN = false;

	if(m_keyboardState[DIK_DIVIDE] & 0x80)
	{
		keys->key_DIVIDE = true;

	} else keys->key_DIVIDE = false;

	if(m_keyboardState[DIK_PERIOD] & 0x80)
	{
		keys->key_PERIOD = true;

	} else keys->key_PERIOD = false;

	if(m_keyboardState[DIK_COLON] & 0x80)
	{
		keys->key_COLON = true;

	} else keys->key_COLON = false;

	return;
}

bool InputClass::IsMouseLeftPressed()
{
	
	if(m_mouseState.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
}

bool InputClass::IsMouseRightPressed()
{
	
	if(m_mouseState.rgbButtons[1] & 0x80)
	{
		return true;
	}

	return false;
}