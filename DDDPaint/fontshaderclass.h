////////////////////////////////////////////////////////////////////////////////
// Filename: fontshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _FONTSHADERCLASS_H_
#define _FONTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11.h>
#include <d3dx11async.h>
#include <d3dx11effect.h>
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: FontShaderClass
////////////////////////////////////////////////////////////////////////////////
class FontShaderClass
{
public:
	FontShaderClass();
	FontShaderClass(const FontShaderClass&);
	~FontShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4, bool);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4, bool);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3DX11Effect* m_effect;
	ID3DX11EffectTechnique* m_technique;
	ID3D11InputLayout* m_layout;

	ID3DX11EffectMatrixVariable* m_worldMatrixPtr;
	ID3DX11EffectMatrixVariable* m_viewMatrixPtr;
	ID3DX11EffectMatrixVariable* m_projectionMatrixPtr;
	ID3DX11EffectShaderResourceVariable* m_texturePtr;
	ID3DX11EffectVectorVariable* m_pixelColorPtr;
	ID3DX11EffectScalarVariable* m_alphaEnabledPtr;
};

#endif