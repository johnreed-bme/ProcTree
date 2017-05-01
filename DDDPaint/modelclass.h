///////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <Importer.hpp>      // C++ importer interface
#include <scene.h>           // Output data structure
#include <postprocess.h>     // Post processing flags
#include <mesh.h>
#include <types.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
//#include "treeclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
public:
	
	struct InstanceType
	{
		D3DXVECTOR4 matrix1;
		D3DXVECTOR4 matrix2;
		D3DXVECTOR4 matrix3;
		D3DXVECTOR4 matrix4;
		FLOAT instance_matrix_offset;
		D3DXVECTOR3 padding;
	};

	struct ModelDataType
	{
		int vertex_count;
		int instance_count;
		vector<D3DXMATRIX> bone_offset_matrices;
		ID3D11ShaderResourceView* texture;
	};

private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR4 bone_weights;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float* bone_weights;
	};

	struct BoneType
	{
		string name;
		D3DXMATRIX bone_offset_matrix;
		D3DXMATRIX bone_inverse_offset_matrix;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, WCHAR*, vector<InstanceType>);
	bool LoadModel(const char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	ModelDataType getModelData();
	int GetVertexCount();
	int GetInstanceCount();
	int GetBoneCount();
	vector<D3DXMATRIX> getBoneOffsetMatrices();
	D3DXMATRIX GetBoneOffsetMatrix(unsigned int);
	D3DXMATRIX GetBoneInverseOffsetMatrix(unsigned int);
	ID3D11ShaderResourceView* GetTexture();

	void setInstanceCount(unsigned int);

private:
	bool InitializeBuffers(ID3D11Device*, vector<InstanceType>);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();
	
	void ReleaseModel();

private:
	ID3D11Buffer *m_vertexBuffer, *m_instanceBuffer;
	unsigned int m_vertexCount, m_instanceCount, m_faceCount, m_boneCount;
	TextureClass* m_Texture;
	ModelType* m_model;
	BoneType* m_bones;
};

#endif