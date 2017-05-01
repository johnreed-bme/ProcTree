////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <time.h>
using namespace std;
///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "inputclass.h"
#include "fontshaderclass.h"
#include "bitmapclass.h"

#include "lightshaderclass.h"
#include "lightclass.h"

#include "cursorclass.h"

#include "treeclass.h"

#include "guiclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(HWND, int,int,int,float,float,KEYBOARD_KEYS,bool*);
	void TransformSelectedBranch(float, float, float);
	void SignalTreeRebuild();

private:
	bool Render(float,float,float,float,float,float,bool*,bool,bool);
	void RenderScene(float,float,float,float,float,float,bool*,string);

public:
	TreeClass* m_Tree;
	GUIClass* m_GUI;
	float boneEditRotX, boneEditRotY, boneEditRotZ;

private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	FontShaderClass* m_FontShader;

	LightShaderClass* m_LightShader;
	LightClass* m_Light;

	CursorClass* m_Cursor;

	D3DXMATRIX baseViewMatrix;
	D3DXMATRIX default_worldMatrix;

	int mouse_frozen_pos[2];
	bool rebuild_tree;
	int my_mem;
	char my_card[128];
};

#endif