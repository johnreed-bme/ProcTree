////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_instanceBuffer = 0;
	m_instanceCount = 0;
	m_Texture = 0;
	m_model = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, WCHAR* textureFilename, vector<InstanceType> instances)
{
	bool result;

	// Initialize the vertex and instance buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device, instances);
	if(!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}


	return true;
}

void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetVertexCount()
{
	return m_vertexCount;
}

int ModelClass::GetBoneCount()
{
	return m_boneCount;
}


int ModelClass::GetInstanceCount()
{
	return m_instanceCount;
}

void ModelClass::setInstanceCount(unsigned int instance_count)
{
	m_instanceCount = instance_count;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device, vector<InstanceType> instances)
{
	VertexType* vertices;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;
	InstanceType* instance_array = new InstanceType[instances.size()];
	int i;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for(i=0; i<m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].bone_weights = D3DXVECTOR4(m_model[i].bone_weights[0], m_model[i].bone_weights[1], m_model[i].bone_weights[2], 0.0f);
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;
	
	
	for (i = 0; i < instances.size(); i++)
	{
		instance_array[i] = instances[i];
	}
	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instance_array;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the instance array now that the instance buffer has been created and loaded.
	delete [] instance_array;
	instance_array = 0;

	return true;
}

bool ModelClass::LoadModel(const char* filename)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	const aiScene* scene = importer.ReadFile( filename,
		//aiProcess_CalcTangentSpace			| 
		aiProcess_Triangulate				|
		//aiProcess_GenSmoothNormals 		|
		//aiProcess_SortByPType				|
		//aiProcess_MakeLeftHanded			|
		aiProcess_FlipUVs					
		//aiProcess_FlipWindingOrder
		);

	// If the import failed, report it
	if( !scene)
	{
		return false;
	}

	aiMesh* imported_mesh = scene->mMeshes[0];

	aiBone** imported_bones = imported_mesh->mBones;
	
	m_boneCount = imported_mesh->mNumBones;

	ofstream Info_proc("info_load.txt", ios::out);
	//Info_proc<<imported_bones[0]->mName.C_Str()<<endl;
	//Info_proc<<imported_bones[0]->mOffsetMatrix<<endl;

	//Info_proc<<m_boneCount<<endl<<endl;

	/*for (int i=0; i<imported_bones[0]->mNumWeights; i++)
	{
		Info_proc<<imported_bones[0]->mWeights[i].mVertexId<<endl;
		Info_proc<<imported_bones[0]->mWeights[i].mWeight<<endl<<endl;
	}*/

	

	m_faceCount = imported_mesh->mNumFaces;
	m_vertexCount = m_faceCount*3;

	m_model = new ModelType[m_vertexCount];
	if(!m_model)
	{
		return false;
	}

	// Fill up the vertex data.
	for (int i=0; i<m_faceCount; i++)
	{
		aiFace current_face = imported_mesh->mFaces[i];

		for (int j=0; j<3; j++)
		{
			int current_index = current_face.mIndices[j];

			m_model[current_index].x = imported_mesh->mVertices[current_index].x;
			m_model[current_index].y = imported_mesh->mVertices[current_index].y;
			m_model[current_index].z = imported_mesh->mVertices[current_index].z;
			m_model[current_index].tu = imported_mesh->mTextureCoords[0][current_index].x;
			m_model[current_index].tv = imported_mesh->mTextureCoords[0][current_index].y;
			m_model[current_index].nx = imported_mesh->mNormals[current_index].x;
			m_model[current_index].ny = imported_mesh->mNormals[current_index].y;
			m_model[current_index].nz = imported_mesh->mNormals[current_index].z;

			m_model[current_index].bone_weights = new float[m_boneCount];

			for (int k=0; k<m_boneCount; k++)
			{
				m_model[current_index].bone_weights[k] = 0.0;
			}
		}
	}

	

	// Fill up the bone weight data for vertices
	m_bones = new BoneType[m_boneCount];

	for (int i=0;  i<m_boneCount; i++)
	{
		Info_proc<<imported_bones[i]->mName.C_Str()<<endl;
		// Fill up bone data
		aiMatrix4x4 offset_matrix_holder = imported_bones[i]->mOffsetMatrix;

		m_bones[i].name = imported_bones[i]->mName.C_Str();
		m_bones[i].bone_offset_matrix = *(new D3DXMATRIX(offset_matrix_holder.a1, offset_matrix_holder.a2, offset_matrix_holder.a3, offset_matrix_holder.a4,
														offset_matrix_holder.b1, offset_matrix_holder.b2, offset_matrix_holder.b3, offset_matrix_holder.b4,
														offset_matrix_holder.c1, offset_matrix_holder.c2, offset_matrix_holder.c3, offset_matrix_holder.c4,
														offset_matrix_holder.d1, offset_matrix_holder.d2, offset_matrix_holder.d3, offset_matrix_holder.d4));
		D3DXMatrixInverse(&m_bones[i].bone_inverse_offset_matrix, 0, &m_bones[i].bone_offset_matrix);

		D3DXMatrixTranspose(&(m_bones[i].bone_offset_matrix), &(m_bones[i].bone_offset_matrix));
		D3DXMatrixTranspose(&(m_bones[i].bone_inverse_offset_matrix), &(m_bones[i].bone_inverse_offset_matrix));

		//Info_proc<<m_bones[i].name<<endl<<offset_matrix_holder.a1<<"    "<<offset_matrix_holder.a2<<"    "<<offset_matrix_holder.a3<<"    "<<offset_matrix_holder.a4<<endl<<offset_matrix_holder.b1<<"    "<<offset_matrix_holder.b2<<"    "<<offset_matrix_holder.b3<<"    "<<offset_matrix_holder.b4<<endl<<offset_matrix_holder.c1<<"    "<<offset_matrix_holder.c2<<"    "<<offset_matrix_holder.c3<<"    "<<offset_matrix_holder.c4<<endl<<offset_matrix_holder.d1<<"    "<<offset_matrix_holder.d2<<"    "<<offset_matrix_holder.d3<<"    "<<offset_matrix_holder.d4<<endl<<endl;

		for (int j=0; j<imported_bones[i]->mNumWeights; j++)
		{
			m_model[imported_bones[i]->mWeights[j].mVertexId].bone_weights[i] = imported_bones[i]->mWeights[j].mWeight;
		}
	}

	/*for (int i=0;  i<m_vertexCount; i++)
	{
		Info_proc<<m_model[i].bone_weights[0]<<"  "<<m_model[i].bone_weights[1]<<"  "<<m_model[i].bone_weights[2]<<"  "<<m_model[i].bone_weights[0]+m_model[i].bone_weights[1]+m_model[i].bone_weights[2]<<endl<<endl;
	}*/

	Info_proc.close();

	imported_mesh = 0;

	ofstream Info_tree("test_tree.txt", ios::out);
	
	Info_tree.close();

	return true;
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseModel()
{
	if(m_model)
	{
		delete [] m_model;
		m_model = 0;
	}

	return;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

void ModelClass::ShutdownBuffers()
{
	// Release the instance buffer.
	if(m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides.
	strides[0] = sizeof(VertexType); 
	strides[1] = sizeof(InstanceType); 

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;	
	bufferPointers[1] = m_instanceBuffer;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

vector<D3DXMATRIX> ModelClass::getBoneOffsetMatrices()
{
	vector<D3DXMATRIX> ret_val;

	for (int i=0; i<m_boneCount; i++)
	{
		ret_val.push_back(m_bones[i].bone_offset_matrix);
	}

	for (int i=m_boneCount; i<10; i++)
	{
		ret_val.push_back(*(new D3DXMATRIX(1.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f)));
	}

	return ret_val;
}

ModelClass::ModelDataType ModelClass::getModelData()
{
	ModelClass::ModelDataType model_data;

	model_data.vertex_count = GetVertexCount();
	model_data.instance_count = GetInstanceCount();
	model_data.bone_offset_matrices = getBoneOffsetMatrices();
	model_data.texture = GetTexture();

	return model_data;
}

D3DXMATRIX ModelClass::GetBoneOffsetMatrix(unsigned int n)
{
	return m_bones[n].bone_offset_matrix;
}

D3DXMATRIX ModelClass::GetBoneInverseOffsetMatrix(unsigned int n)
{
	return m_bones[n].bone_inverse_offset_matrix;
}