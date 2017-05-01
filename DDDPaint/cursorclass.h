////////////////////////////////////////////////////////////////////////////////
// Filename: cursorclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CURSORCLASS_H_
#define _CURSORCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: CursorClass
////////////////////////////////////////////////////////////////////////////////
class CursorClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	CursorClass();
	CursorClass(const CursorClass&);
	~CursorClass();
	
	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	bool SetMousePosition(ID3D11Device*, int, int);
	void GetMousePosition(int*);

	bool SetSize(ID3D11Device*, int,int);

	void GetScreenRes(int*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11Device*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
	int m_currentPosX, m_currentPosY;
	bool sizeChangedFlag;
};

#endif