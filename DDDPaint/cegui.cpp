////////////////////////////////////////////////////////////////////////////////
// Filename: cegui.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cegui.h"

GUI::GUI()
{
	m_Renderer = 0;
}


GUI::GUI(const GUI& other)
{
}


GUI::~GUI()
{
}

bool GUI::Initialize(ID3D10Device* device){

	m_Renderer = &CEGUI::Direct3D10Renderer::bootstrapSystem(device);

	// Load the scheme
	CEGUI::SchemeManager::getSingleton().create( "TaharezLook.scheme" );

	// Set the defaults
	CEGUI::System::getSingleton().setDefaultFont( "DejaVuSans-10" );
	CEGUI::System::getSingleton().setDefaultMouseCursor( "TaharezLook", "MouseArrow" );

	myRoot = CEGUI::WindowManager::getSingleton().createWindow( "DefaultWindow", "_MasterRoot" );
	CEGUI::System::getSingleton().setGUISheet( myRoot );

	return true;
}

void Shutdown(){

}

void Render(){
	 // draw GUI
	CEGUI::System::getSingleton().renderGUI();
}