////////////////////////////////////////////////////////////////////////////////
// Filename: treeclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _TREECLASS_H_
#define _TREECLASS_H_

//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <d3dx10math.h>
#include <iostream>
#include <queue>
#include <deque>
#include <time.h>
using namespace std;

#include "componenthandlerclass.h"
#include "modelclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TreeClass
////////////////////////////////////////////////////////////////////////////////
class TreeClass
{
public:
	struct TreeInstanceData
	{
		D3DXMATRIX offset_matrix;
		D3DXMATRIX offset_matrix_inverse;
		vector<D3DXMATRIX> bone_transformation_matrices;
		unsigned int instance_matrix_offset;
	};

	struct TreeNode
	{
		TreeInstanceData instance_data;
		deque<TreeNode*> child_nodes;
		TreeNode* parent_node;
		unsigned int parent_bone;
		unsigned int max_child_count;
		unsigned int modelNum;
	};

public:
	TreeClass();
	TreeClass(const TreeClass&);
	~TreeClass();

	bool Initialize(ID3D11Device*, HWND, char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*, unsigned int);

	ModelClass* GetModel(unsigned int);

	vector<ModelClass::InstanceType> getInstanceData(unsigned int);
	vector<D3DXMATRIX> getBoneMatrices();

	static TreeInstanceData read_instance_data(TreeNode*);
	bool TransformBranch(unsigned int, unsigned int, D3DXMATRIX);
	static bool write_instance_data(TreeNode*, TreeInstanceData);
	static bool append_node(TreeNode*, TreeNode*);
	static TreeNode newNode(unsigned int, unsigned int, TreeNode*, unsigned int);

	int GetInstanceCount(unsigned int);
	int GetAllInstanceCount();
	TreeNode GetNode(int);

private:
	bool buildTree();
	void TransformBranch(D3DXMATRIX, TreeNode*, unsigned int);

public:
	int selected_node, selected_bone;
	TreeNode root_node;
	ComponentHandlerClass m_Components;

private:
	vector<D3DXMATRIX> bone_trans_matrix;
	int all_instance_count;
};

#endif