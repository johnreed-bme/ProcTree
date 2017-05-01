////////////////////////////////////////////////////////////////////////////////
// Filename: guiclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GUICLASS_H_
#define _GUICLASS_H_

#include <AntTweakBar.h>
#include <d3d11.h>

#include "inputclass.h"
#include "treeclass.h"

class GraphicsClass;

class GUIClass {

public:
	GUIClass();
	GUIClass(const GUIClass&);
	~GUIClass();

	bool Initialize(int, int, ID3D11Device*, HWND,  GraphicsClass*);
	void Shutdown();
	void DrawGUI();

	int InputHandling(int, int, int, KEYBOARD_KEYS, bool*);

public:
	HWND hwnd;

	float myVar;
	TwBar *myBar;
};

#endif
