////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "guiclass.h"
#include "graphicsclass.h"

GUIClass::GUIClass()
{
}


GUIClass::GUIClass(const GUIClass& other)
{
}


GUIClass::~GUIClass()
{
}

// callback for randomize tree button
void TW_CALL RandomizeTreeCB(void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);
	
	graphics->SignalTreeRebuild();
}

// callbacks for tree node selection
void TW_CALL SetSelectedTreeNodeCB(const void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);
	graphics->m_Tree->selected_node = *static_cast<const int *>(value);

	graphics->m_Tree->selected_bone = 1;
	static int max_bones;
	max_bones = graphics->m_Tree->m_Components.components[graphics->m_Tree->GetNode(graphics->m_Tree->selected_node).modelNum].model_data->GetBoneCount()-1;
	TwSetParam(graphics->m_GUI->myBar, "Bone selected", "max", TW_PARAM_INT32, 1, &max_bones);

	graphics->boneEditRotX = 0.0f;
	graphics->boneEditRotY = 0.0f;
	graphics->boneEditRotZ = 0.0f;
}

void TW_CALL GetSelectedTreeNodeCB(void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);
    *static_cast<int *>(value) = graphics->m_Tree->selected_node;
}

// callbacks for bone selection
void TW_CALL SetSelectedBoneCB(const void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);
    graphics->m_Tree->selected_bone = *static_cast<const int *>(value);

	graphics->boneEditRotX = 0.0f;
	graphics->boneEditRotY = 0.0f;
	graphics->boneEditRotZ = 0.0f;
}

void TW_CALL GetSelectedBoneCB(void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);
    *static_cast<int *>(value) = graphics->m_Tree->selected_bone;
}

// bone rotation callbacks for X axis
void TW_CALL SetBoneRotXCB(const void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);

	float deltaX = *static_cast<const float *>(value) - graphics->boneEditRotX;

    graphics->boneEditRotX = *static_cast<const float *>(value);

	graphics->TransformSelectedBranch(deltaX, 0.0f, 0.0f);
}

void TW_CALL GetBoneRotXCB(void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);
    *static_cast<float *>(value) = graphics->boneEditRotX;
}

// bone rotation callbacks for Y axis
void TW_CALL SetBoneRotYCB(const void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);

	float deltaY = *static_cast<const float *>(value) - graphics->boneEditRotY;

    graphics->boneEditRotY = *static_cast<const float *>(value);

	graphics->TransformSelectedBranch(0.0f, deltaY, 0.0f);
}

void TW_CALL GetBoneRotYCB(void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);
    *static_cast<float *>(value) = graphics->boneEditRotY;
}

// bone rotation callbacks for Z axis
void TW_CALL SetBoneRotZCB(const void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);

	float deltaZ = *static_cast<const float *>(value) - graphics->boneEditRotZ;

    graphics->boneEditRotZ = *static_cast<const float *>(value);

	graphics->TransformSelectedBranch(0.0f, 0.0f, deltaZ);
}

void TW_CALL GetBoneRotZCB(void *value, void *clientData)
{
    GraphicsClass *graphics = static_cast<GraphicsClass *>(clientData);
    *static_cast<float *>(value) = graphics->boneEditRotZ;
}

bool GUIClass::Initialize(int screenWidth, int screenHeight, ID3D11Device* d3dDevice, HWND hwnd, GraphicsClass* graphics)
{
	TwInit(TW_DIRECT3D11, d3dDevice);

	TwWindowSize(screenWidth, screenHeight);

	this->hwnd = hwnd;

	myBar = TwNewBar("Transform");
	
	string max_tree_nodes = to_string(static_cast<long long>(graphics->m_Tree->GetAllInstanceCount() - 1));
	string properties = " min=0 max=" + max_tree_nodes + " step=1 ";
	TwAddVarCB(myBar, "Tree node selected", TW_TYPE_INT32, SetSelectedTreeNodeCB, GetSelectedTreeNodeCB, graphics, properties.c_str());

	string max_bones = to_string(static_cast<long long>(graphics->m_Tree->GetNode(graphics->m_Tree->selected_node).child_nodes.size()));
	properties = " min=1 max=" + max_bones + " step=1 ";
	TwAddVarCB(myBar, "Bone selected", TW_TYPE_INT32, SetSelectedBoneCB, GetSelectedBoneCB, graphics, properties.c_str());

	TwAddVarCB(myBar, "Bone rot X", TW_TYPE_FLOAT, SetBoneRotXCB, GetBoneRotXCB, graphics, " min=-180.0 max=180.0 step=1.0 ");
	TwAddVarCB(myBar, "Bone rot Y", TW_TYPE_FLOAT, SetBoneRotYCB, GetBoneRotYCB, graphics, " min=-180.0 max=180.0 step=1.0 ");
	TwAddVarCB(myBar, "Bone rot Z", TW_TYPE_FLOAT, SetBoneRotZCB, GetBoneRotZCB, graphics, " min=-180.0 max=180.0 step=1.0 ");

	TwAddButton(myBar, "Randomize Tree", RandomizeTreeCB, graphics, " label='Randomize Tree' ");

	return true;
}

void GUIClass::Shutdown()
{
	TwTerminate();
}

void GUIClass::DrawGUI()
{
	TwDraw();  // draw the tweak bar(s)
}

int GUIClass::InputHandling(int mouseX, int mouseY, int mouseZ, KEYBOARD_KEYS keys, bool* mouse_keys)
{
	static bool mouse_left_button = false;
	static bool mouse_right_button = false;
	static int mouseCoordX = 0;
	static int mouseCoordY = 0;
	static int mouseCoordZ = 0;
	int handled = 0;

	if ((mouseCoordX != mouseX) || (mouseCoordY != mouseY))
	{
		mouseCoordX = mouseX;
		mouseCoordY = mouseY;

		handled = TwMouseMotion(mouseCoordX-50, mouseCoordY);
	}

	if (!mouse_left_button && mouse_keys[0])
	{
		mouse_left_button = true;
		handled = TwMouseButton(TW_MOUSE_PRESSED, TW_MOUSE_LEFT);
	}

	if (mouse_left_button && !mouse_keys[0])
	{
		mouse_left_button = false;
		handled = TwMouseButton(TW_MOUSE_RELEASED, TW_MOUSE_LEFT);
	}

	return handled;
}