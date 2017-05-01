////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//////////////
// INCLUDES //
//////////////
#include <dinput.h>

// Struct for keyboard
typedef struct KEYBOARD_KEYS{
	bool key_0;
	bool key_1;
	bool key_2;
	bool key_3;
	bool key_4;
	bool key_5;
	bool key_6;
	bool key_7;
	bool key_8;
	bool key_9;
	bool key_A;
	bool key_B;
	bool key_C;
	bool key_D;
	bool key_E;
	bool key_F;
	bool key_G;
	bool key_H;
	bool key_I;
	bool key_J;
	bool key_K;
	bool key_L;
	bool key_M;
	bool key_N;
	bool key_O;
	bool key_P;
	bool key_Q;
	bool key_R;
	bool key_S;
	bool key_T;
	bool key_U;
	bool key_V;
	bool key_W;
	bool key_X;
	bool key_Y;
	bool key_Z;
	bool key_SPACE;
	bool key_RETURN;
	bool key_LCONTROL;
	bool key_SHIFT;
	bool key_BACK;
	bool key_DELETE;
	bool key_TAB;
	bool key_LEFT;
	bool key_RIGHT;
	bool key_UP;
	bool key_DOWN;
	bool key_DIVIDE;
	bool key_PERIOD;
	bool key_COLON;
} KEYBOARD_KEYS;

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool InputClass::Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&, int&, float&, float&);
	bool IsMouseLeftPressed();
	bool IsMouseRightPressed();

	void IsKeyPressed(bool*);
	void IsKeyPressed(KEYBOARD_KEYS*);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY, m_mouseZ;
	float m_mouseDeltaX, m_mouseDeltaY;
};

#endif