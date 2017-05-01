////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

#ifndef UNICODE  
typedef std::string String; 
#else
typedef std::wstring String; 
#endif

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	//m_D3D_hd = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Tree = 0;
	m_GUI = 0;
	m_FontShader = 0;

	m_LightShader = 0;
	m_Light = 0;

	m_Cursor = 0;
	mouse_frozen_pos[0]=mouse_frozen_pos[1]=0;

}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	stringstream str;
	string s;
	bool result;

	boneEditRotX = boneEditRotY = boneEditRotZ = 0.0f;

	rebuild_tree = false;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_D3D->GetWorldMatrix(default_worldMatrix);

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(50.0f, 0.0f, 0.0f);

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// Create the tree object.
	m_Tree = new TreeClass;
	if(!m_Tree)
	{
		return false;
	}

	// Initialize the tree object.
	result = m_Tree->Initialize(m_D3D->GetDevice(), hwnd, "../DDDPaint/data/branch_bones.dae", L"../DDDPaint/data/dragon.png");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the tree object.", L"Error", MB_OK);
		return false;
	}

	// Create the GUI object.
	m_GUI = new GUIClass;
	if(!m_GUI)
	{
		return false;
	}

	// Initialize the GUI object.
	result = m_GUI->Initialize(screenWidth, screenHeight, m_D3D->GetDevice(), hwnd, this);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the GUI object.", L"Error", MB_OK);
		return false;
	}

	// Create the cursor object.
	m_Cursor = new CursorClass;
	if(!m_Cursor)
	{
		return false;
	}

	// Initialize the cursor object.
	result = m_Cursor->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"../DDDPaint/data/test3.png", 32, 32);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the cursor object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if(!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(-1.0f, 0.0f, 1.0f);

	// Uotputting video card info
	ofstream Info_proc("info_proc.txt", ios::out);
	m_D3D->GetVideoCardInfo(my_card,my_mem);
	Info_proc<<my_card<<'\n'<<my_mem;
	Info_proc.close();

	str << my_mem;

	return true;
}


void GraphicsClass::Shutdown()
{

	// Release the cursor object.
	if(m_Cursor)
	{
		m_Cursor->Shutdown();
		delete m_Cursor;
		m_Cursor = 0;
	}

	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the font shader object.
	if(m_FontShader)
	{
	m_FontShader->Shutdown();
	delete m_FontShader;
	m_FontShader = 0;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the tree object.
	if(m_Tree)
	{
		m_Tree->Shutdown();
		delete m_Tree;
		m_Tree = 0;
	}

	// Release the GUI object.
	if(m_GUI)
	{
		m_GUI->Shutdown();
		delete m_GUI;
		m_GUI = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	/*if(m_D3D_hd)
	{
	m_D3D_hd->Shutdown();
	delete m_D3D_hd;
	m_D3D_hd = 0;
	}*/

	return;
}

void GraphicsClass::TransformSelectedBranch(float RotX, float RotY, float RotZ)
{
	D3DXMATRIX transform;
	D3DXMATRIX rotateX;
	D3DXMATRIX rotateY;
	D3DXMATRIX rotateZ;

	D3DXMatrixRotationX(&rotateX, RotX * 0.0174532925f);
	D3DXMatrixRotationY(&rotateY, RotY * 0.0174532925f);
	D3DXMatrixRotationZ(&rotateZ, RotZ * 0.0174532925f);
	transform = rotateX * rotateZ * rotateY;

	m_Tree->TransformBranch(m_Tree->selected_node, m_Tree->selected_bone, transform);
}

void GraphicsClass::SignalTreeRebuild()
{
	rebuild_tree = true;
}

bool GraphicsClass::Frame(HWND hwnd, int mouseX, int mouseY, int mouseZ, float mouseDeltaX, float mouseDeltaY, KEYBOARD_KEYS keys, bool* mouse)
{
	bool result;
	int handled_by_gui = 0;
	static bool enableDrawing = true;
	static bool enableStencilCursor = true;
	static bool first_frame_flag = true;
	static bool prev_frame_mouse_down = false;
	static clock_t ticks_elapsed;
	static float cameraRotationX = 0.0f;
	static float cameraRotationY = -90.0f;
	static float cameraRotationZ = 0.0f;
	float cameraMoveX = 0.0f;
	float cameraMoveY = 0.0f;
	float cameraMoveZ = 0.0f;
	static int prev_mouseZ = 0;
	int temp[2];

	static float time_elapsed = 0.0f;
	time_elapsed += ((clock()-(float)ticks_elapsed)/CLOCKS_PER_SEC);
	ticks_elapsed = clock();

	handled_by_gui = m_GUI->InputHandling(mouseX, mouseY, mouseZ, keys, mouse);
	if (!handled_by_gui)
	{

		if (mouse[0]){
			if (!prev_frame_mouse_down){
				prev_frame_mouse_down = true;
			}
			// Update the cameraRotation variable each frame.
			cameraRotationY -= (float)D3DX_PI * 0.02f * mouseDeltaX;
			if(cameraRotationY > 360.0f)
			{
				cameraRotationY -= 360.0f;
			}
			if(cameraRotationY < -360.0f)
			{
				cameraRotationY += 360.0f;
			}

			cameraRotationX -= (float)D3DX_PI * 0.02f * mouseDeltaY;
			if(cameraRotationX > 360.0f)
			{
				cameraRotationX -= 360.0f;
			}
			if(cameraRotationX < -360.0f)
			{
				cameraRotationX += 360.0f;
			}

		} else {
			if (prev_frame_mouse_down){
				prev_frame_mouse_down = false;
			}
		}
	}

	if (enableStencilCursor)
		if (keys.key_LCONTROL) {

			if (keys.key_S){
			}

			if (keys.key_Z) {

			}

		} else {

			//m_Cursor->SetSize(m_D3D->GetDevice(), temp[0], temp[1]);

			if (time_elapsed > 0.0002f) {
				

				if (keys.key_W){
					// Update the cameraMove variable each frame.
					cameraMoveZ += 0.02f * (time_elapsed / 0.0002f);
				}
				if (keys.key_S) {
					// Update the cameraMove variable each frame.
					cameraMoveZ -= 0.02f * (time_elapsed / 0.0002f);
				}

				if (keys.key_A){
					// Update the cameraMove variable each frame.
					cameraMoveX -= 0.02f * (time_elapsed / 0.0002f);
				}
				if (keys.key_D) {
					// Update the cameraMove variable each frame.
					cameraMoveX += 0.02f * (time_elapsed / 0.0002f);
				}

				if (keys.key_RIGHT){
				}
				if (keys.key_LEFT) {
				}

				if (keys.key_UP){
				}
				if (keys.key_DOWN) {
				}

				if (keys.key_E){
					cameraMoveY += 0.02f * (time_elapsed / 0.0002f);
				}
				if (keys.key_Q) {
					cameraMoveY -= 0.02f * (time_elapsed / 0.0002f);
				}
				time_elapsed = 0.0f;
			}

			if (keys.key_DELETE) {
				cameraRotationX = 10.0f;
				cameraRotationZ = 0.0;
				cameraRotationY = -90.0f;
				m_Camera->SetPosition(100.0f, 50.0f, 0.0f);
			}

		}

		if (first_frame_flag) {
			if (first_frame_flag) first_frame_flag = false;
		}

		// Set the location of the mouse.
		result = m_Cursor->SetMousePosition(m_D3D->GetDevice(), mouseX, mouseY);
		if(!result)
		{
			return false;
		}

		// Reset tree if requested
		if (rebuild_tree)
		{
			// Release the tree object.
			if(m_Tree)
			{
				m_Tree->Shutdown();
				delete m_Tree;
				m_Tree = 0;
			}

			// Create the tree object.
			m_Tree = new TreeClass;
			if(!m_Tree)
			{
				return false;
			}

			// Initialize the tree object.
			result = m_Tree->Initialize(m_D3D->GetDevice(), hwnd, "null", L"null");
			if(!result)
			{
				MessageBox(hwnd, L"Could not initialize the tree object.", L"Error", MB_OK);
				return false;
			}

			rebuild_tree = false;
		}

		// Render the graphics scene.
		result = Render(cameraRotationX, cameraRotationY, cameraRotationZ, cameraMoveX, cameraMoveY,
			cameraMoveZ, mouse, enableDrawing, enableStencilCursor);
		if(!result)
		{
			return false;
		}

		if (enableStencilCursor){
			mouse_frozen_pos[0] = mouseX;
			mouse_frozen_pos[1] = mouseY;
		}

		return true;
}

void GraphicsClass::RenderScene(float cameraRotationX, float cameraRotationY, float cameraRotationZ, float cameraMoveX, float cameraMoveY,
	float cameraMoveZ, bool *mousePressed, string ShaderName)
{
	bool result;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX viewMatrix, projectionMatrix, orthoMatrix;
	D3DXMATRIX cameraRotationmatrix;
	D3DXMATRIX scalematrix;
	D3DXMATRIX translatematrix;
	D3DXMATRIX transform;
	D3DXMATRIX rotateX;
	D3DXMATRIX rotateY;
	D3DXMATRIX rotateZ;
	D3DXMATRIX scale;
	D3DXMATRIX translate;
	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 cameraMoveVector;
	D3DXVECTOR4 rotatedcameraMoveVector;
	static clock_t ticks_elapsed = clock();
	//static bool first_frame_flag = true;

	lookAt.x = -1.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	cameraMoveVector.x = cameraMoveX;
	cameraMoveVector.y = cameraMoveY;
	cameraMoveVector.z = cameraMoveZ;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.2f, 0.2f, 0.2f, 0.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->SetRotation(cameraRotationX,cameraRotationY,cameraRotationZ);
	D3DXMatrixRotationYawPitchRoll(&cameraRotationmatrix, cameraRotationY* 0.0174532925f, cameraRotationX* 0.0174532925f, cameraRotationZ* 0.0174532925f);
	D3DXVec3Transform(&rotatedcameraMoveVector,&cameraMoveVector,&cameraRotationmatrix);
	m_Camera->UpdatePosition(rotatedcameraMoveVector.x,rotatedcameraMoveVector.y,rotatedcameraMoveVector.z);
	m_Camera->Render();

	// Make the light source move together with the camera
	//D3DXVec3TransformCoord(&lookAt, &lookAt, &cameraRotationmatrix);
	//m_Light->SetDirection(lookAt.x, lookAt.y, lookAt.z);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// Rotate the world matrix by the cameraRotation value
	/*
	D3DXMatrixcameraRotationYawPitchRoll(&worldMatrix, cameraRotationX, cameraRotationY, cameraRotationZ);
	D3DXMatrixScaling(&scalematrix, 1.0f, 1.0f, 1.0f);
	D3DXMatrixTranslation(&translatematrix,cameraMoveX,cameraMoveY,cameraMoveZ);
	viewMatrix *= scalematrix * translatematrix;
	*/

	static bool test_flag = false;

	static float rotX = 10.0f;
	static float degree = 1.0f;
	static float rotSum = 0.0f;
	static bool dir = true;

	if (((clock()-(float)ticks_elapsed)/CLOCKS_PER_SEC) > 0.0161f) {
		ticks_elapsed = clock();

		//rotX = sin(degree) * 90.0f;

		if (rotSum<30.0f){
			rotSum += 1.0f;
			test_flag = false;
		}

		if (rotSum >= 30.0f) {
			rotSum = 0.0f;
			degree *= -1.0f;
		}

		D3DXMatrixTranslation(&translate, 0.0f, 0.0f, 0.0f);
		D3DXMatrixScaling(&scale, 1.0f, 1.0f, 1.0f);
		D3DXMatrixRotationX(&rotateX, degree * 0.0174532925f);
		D3DXMatrixRotationY(&rotateY, 0.0f * 0.0174532925f);
		D3DXMatrixRotationZ(&rotateZ, 0.0f * 0.0174532925f);
		transform = rotateX * rotateZ * rotateY;

		if (!test_flag)
		{
			//result = m_Tree->TransformBranch(m_Tree->selected_node, m_Tree->selected_bone, transform);
			test_flag = true;
		}
	}


	vector<D3DXVECTOR3> boneTransformData;
	D3DXVECTOR3 boneTransform;
	boneTransform.x = 0.0f;
	boneTransform.y = 0.0f;
	boneTransform.z = rotX * 0.0174532925f;
	for (int i=0; i<10; i++)
	{
		boneTransformData.push_back(boneTransform);
	}

	
	for (int i = 0; i < m_Tree->m_Components.components.size(); ++i) {

		if (m_Tree->m_Components.components[i].instance_count == 0) {
			continue;
		}

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Tree->Render(m_D3D->GetDeviceContext(), i);

		if (!(ShaderName.compare("lightshader"))){
			// Render the model using the light shader.

			result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Tree->GetModel(i)->GetVertexCount(), m_Tree->GetModel(i)->GetInstanceCount(), 
				worldMatrix, viewMatrix, projectionMatrix, m_Tree->GetModel(i)->getBoneOffsetMatrices(), boneTransformData,
				m_Tree->getBoneMatrices(), 
				m_Tree->GetModel(i)->GetTexture(), NULL, m_Light->GetDirection(), m_Camera->GetPosition(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
			if(!result)
			{
				return;
			}
			ID3D11ShaderResourceView* srvs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {0};
			m_D3D->GetDeviceContext()->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, srvs);
		} else {
			return;
		}
	}

	return;
}


bool GraphicsClass::Render(float rotationX, float rotationY, float rotationZ, float offsetX, float offsetY,
	float offsetZ, bool *mousePressed, bool enableDrawing, bool enableStencilCursor)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	D3DXMATRIX rotX, rotY;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

	// Render the scene as normal to the back buffer.
	RenderScene(rotationX, rotationY, rotationZ, offsetX, offsetY, offsetZ, mousePressed, "lightshader");

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);
	
	
	// Turn off the Z buffer to begin all 2D rendering.
	m_D3D->TurnZBufferOff();

	if (!enableStencilCursor) {

		m_Cursor->SetMousePosition(m_D3D->GetDevice(), mouse_frozen_pos[0], mouse_frozen_pos[1]);

	}

	// Put the cursor vertex and index buffers on the graphics pipeline to prepare them for drawing.
	result = m_Cursor->Render(m_D3D->GetDeviceContext());
	if(!result)
	{
	return false;
	}

	m_FontShader->Render(m_D3D->GetDeviceContext(), m_Cursor->GetIndexCount(), default_worldMatrix, baseViewMatrix, orthoMatrix,
	m_Cursor->GetTexture(), D3DXVECTOR4(0.0f, 0.7f, 0.7f, 0.0f), true);
	

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_D3D->TurnZBufferOn();
	

	m_GUI->DrawGUI();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}