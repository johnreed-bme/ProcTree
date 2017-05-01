////////////////////////////////////////////////////////////////////////////////
// Filename: lightshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTSHADERCLASS_H_
#define _LIGHTSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <vector>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: LightShaderClass
////////////////////////////////////////////////////////////////////////////////
class LightShaderClass
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX bone_transformation_offset[10];	// offset matrices for bone transformations
		D3DXMATRIX bone_inverse_transformation_offset[10];
		D3DXVECTOR3 bone_transformation[10];		// rotation data for bone rotations, x,y,z data for the x,y,z axis rotations (in radians)
		D3DXVECTOR2 padding;
	};
	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR3 lightDirection;
		D3DXVECTOR3 cameraPosition;
		D3DXVECTOR2 padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};
	struct testBufferType
	{
		D3DXMATRIX data;
	};
public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, vector<D3DXMATRIX>, vector<D3DXVECTOR3>, vector<D3DXMATRIX>,
			      ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, vector<D3DXMATRIX>, vector<D3DXVECTOR3>, vector<D3DXMATRIX>,
					   ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4, 
					   D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_testBuffer;
	ID3D11ShaderResourceView* m_testBufferView;
	ID3D11SamplerState* m_sampleState;
	ID3D11BlendState* m_blendState;
	ID3D11DepthStencilState* m_depthState;
	ID3D11RasterizerState* m_rasterState;
};

#endif
