////////////////////////////////////////////////////////////////////////////////
// Filename: treeclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "treeclass.h"

TreeClass::TreeClass()
{
	all_instance_count = 0;
}

TreeClass::TreeClass(const TreeClass& other)
{
}

TreeClass::~TreeClass()
{
}

bool TreeClass::Initialize(ID3D11Device* device, HWND hwnd, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	// Load in the components.
	result = m_Components.Initialize("../DDDPaint/data/components.txt");
	if(!result)
	{
		return false;
	}

	for (int i = 0; i < m_Components.components.size(); ++i) {
		m_Components.components[i].model_data = new ModelClass;
		m_Components.LoadModel(i);
	}

	// Build tree
	buildTree();

	for (int i = 0; i < m_Components.components.size(); ++i) {
		m_Components.InitModel(i, device, getInstanceData(i));
	}

	selected_node = 0;
	selected_bone = 1;
	
	return true;
}

void TreeClass::Shutdown()
{
	// TODO: avoid memory leak: traverse tree and use delete on nodes
	m_Components.Shutdown();
	
	return;
}

void TreeClass::Render(ID3D11DeviceContext* deviceContext, unsigned int modelNum)
{

	m_Components.components[modelNum].model_data->Render(deviceContext);

	return;
}

TreeClass::TreeNode TreeClass::GetNode(int node_number)
{
	TreeClass::TreeNode* node_iterator = &root_node;
	queue<TreeClass::TreeNode*> queue_current, queue_next;
	queue<TreeClass::TreeNode*>().swap(queue_current);
	queue<TreeClass::TreeNode*>().swap(queue_next);
	int i = 0;
	while (true)
	{
		// data operations on actual node
		
		if (i == node_number)
		{
			return *node_iterator;
		}

		i++;

		for (int j=0; j<node_iterator->child_nodes.size(); j++)
		{
			queue_next.push(node_iterator->child_nodes[j]);
		}

		if (!queue_current.size())
		{
			if (!queue_next.size())
			{
				break;
			}

			queue_current = queue_next;
			queue<TreeClass::TreeNode*>().swap(queue_next);
		}

		node_iterator = queue_current.front();
		queue_current.pop();
	}
	return *node_iterator;
}

bool TreeClass::TransformBranch(unsigned int node_to_transform, unsigned int bone_to_transform, D3DXMATRIX transform)
{
	TreeClass::TreeNode* node_iterator = &root_node;
	queue<TreeClass::TreeNode*> queue_current, queue_next;
	queue<TreeClass::TreeNode*>().swap(queue_current);
	queue<TreeClass::TreeNode*>().swap(queue_next);
	int i = 0;
	while (true)
	{
		// data operations on actual node
		
		if (i == node_to_transform)
		{
			TransformBranch(transform, node_iterator, bone_to_transform);
			return true;
		}

		i++;

		for (int j=0; j<node_iterator->child_nodes.size(); j++)
		{
			queue_next.push(node_iterator->child_nodes[j]);
		}

		if (!queue_current.size())
		{
			if (!queue_next.size())
			{
				return false;
			}

			queue_current = queue_next;
			queue<TreeClass::TreeNode*>().swap(queue_next);
		}

		node_iterator = queue_current.front();
		queue_current.pop();
	}

	return false;
}

void TreeClass::TransformBranch(D3DXMATRIX transform, TreeNode* node_to_transform, unsigned int bone_to_transform)
{
	TreeClass::TreeNode* node_iterator = node_to_transform;
	queue<TreeClass::TreeNode*> queue_current, queue_next;
	queue<TreeClass::TreeNode*>().swap(queue_current);
	queue<TreeClass::TreeNode*>().swap(queue_next);

	D3DXMATRIX node_transform = m_Components.components[node_iterator->modelNum].model_data->GetBoneOffsetMatrix(bone_to_transform) * transform * m_Components.components[node_iterator->modelNum].model_data->GetBoneInverseOffsetMatrix(bone_to_transform);
	node_iterator->instance_data.bone_transformation_matrices[bone_to_transform] *= node_transform;

	D3DXMATRIX transformed_node_offset = node_iterator->instance_data.offset_matrix;
	//D3DXMATRIX transformed_node_offset_inverse = node_iterator->instance_data.offset_matrix_inverse;
	D3DXMATRIX transformed_node_offset_inverse;
	D3DXMatrixInverse(&transformed_node_offset_inverse, 0, &transformed_node_offset);
	node_iterator->instance_data.offset_matrix_inverse = transformed_node_offset_inverse;
	D3DXMATRIX node_children_transform = transformed_node_offset_inverse * node_transform * transformed_node_offset;

	if (node_iterator->child_nodes.empty())
	{
		return;
	}

	node_iterator = node_to_transform->child_nodes[bone_to_transform - 1];

	unsigned int n;
	while (true)
	{
		// data operations on actual node

		node_iterator->instance_data.offset_matrix *= node_children_transform;
		
		for (int i=0; i<m_Components.components[node_iterator->modelNum].model_data->GetBoneCount(); i++)
		{
			n = node_iterator->instance_data.instance_matrix_offset + i;
			bone_trans_matrix[n] *= node_children_transform;
		}

		for (int i=0; i<node_iterator->child_nodes.size(); i++)
		{
			queue_next.push(node_iterator->child_nodes[i]);
		}

		if (!queue_current.size())
		{
			if (!queue_next.size())
			{
				break;
			}

			queue_current = queue_next;
			queue<TreeClass::TreeNode*>().swap(queue_next);
		}

		node_iterator = queue_current.front();
		queue_current.pop();
	}
}

vector<ModelClass::InstanceType> TreeClass::getInstanceData(unsigned int modelNum)
{
	vector<ModelClass::InstanceType> instances;

	ModelClass::InstanceType actual_data;
	D3DXMATRIX transform;

	TreeClass::TreeNode* node_iterator = &root_node;
	queue<TreeClass::TreeNode*> queue_current, queue_next;
	queue<TreeClass::TreeNode*>().swap(queue_current);
	queue<TreeClass::TreeNode*>().swap(queue_next);

	while (true)
	{
		// data operations on actual node
		if (node_iterator->modelNum == modelNum) {
			transform = read_instance_data(node_iterator).offset_matrix;
			actual_data.matrix1 = D3DXVECTOR4(transform._11, transform._12, transform._13, transform._14);
			actual_data.matrix2 = D3DXVECTOR4(transform._21, transform._22, transform._23, transform._24);
			actual_data.matrix3 = D3DXVECTOR4(transform._31, transform._32, transform._33, transform._34);
			actual_data.matrix4 = D3DXVECTOR4(transform._41, transform._42, transform._43, transform._44);
			actual_data.instance_matrix_offset = read_instance_data(node_iterator).instance_matrix_offset;

			instances.push_back(actual_data);
		}

		for (int i=0; i<node_iterator->child_nodes.size(); i++)
		{
			queue_next.push(node_iterator->child_nodes[i]);
		}

		if (!queue_current.size())
		{
			if (!queue_next.size())
			{
				break;
			}

			queue_current = queue_next;
			queue<TreeClass::TreeNode*>().swap(queue_next);
		}

		node_iterator = queue_current.front();
		queue_current.pop();
	}
	return instances;
}

vector<D3DXMATRIX> TreeClass::getBoneMatrices()
{
	vector<D3DXMATRIX> bone_trans_matrices = bone_trans_matrix;

	TreeClass::TreeNode* node_iterator = &root_node;
	TreeClass::TreeNode* ancestor_node = NULL;
	queue<TreeClass::TreeNode*> queue_current, queue_next;
	queue<TreeClass::TreeNode*>().swap(queue_current);
	queue<TreeClass::TreeNode*>().swap(queue_next);
	while (true)
	{
		// data operations on actual node

		for (int j=0; j<m_Components.components[node_iterator->modelNum].model_data->GetBoneCount(); j++)
		{
			unsigned int n = node_iterator->instance_data.instance_matrix_offset + j;
			bone_trans_matrices[n] =  node_iterator->instance_data.bone_transformation_matrices[j] * bone_trans_matrices[n];
		}

		for (int j=0; j<node_iterator->child_nodes.size(); j++)
		{
			queue_next.push(node_iterator->child_nodes[j]);
		}

		if (!queue_current.size())
		{
			if (!queue_next.size())
			{
				break;
			}

			queue_current = queue_next;
			queue<TreeClass::TreeNode*>().swap(queue_next);
		}

		node_iterator = queue_current.front();
		queue_current.pop();
	}

	return bone_trans_matrices;
}

TreeClass::TreeInstanceData TreeClass::read_instance_data(TreeNode* node)
{
	return node->instance_data;
}

bool TreeClass::write_instance_data(TreeNode* node, TreeInstanceData data)
{
	if (node)
	{
		node->instance_data = data;
		return true;
	}
	else
	{
		return false;
	}
}

bool TreeClass::append_node(TreeNode* base_node, TreeNode* new_node)
{
	if (base_node && base_node->child_nodes.size() < base_node->max_child_count)
	{
		base_node->child_nodes.push_back(new_node);
		return true;
	}
	else
	{
		return false;
	}
}

TreeClass::TreeNode TreeClass::newNode(unsigned int max_count, unsigned int parent_bone, TreeClass::TreeNode* parent_node, unsigned int modelNum)
{
	TreeClass::TreeNode new_node;

	new_node.child_nodes = deque<TreeNode*>();
	new_node.max_child_count = max_count;
	new_node.parent_node = parent_node;
	new_node.parent_bone = parent_bone;
	new_node.modelNum = modelNum;

	return new_node;
}

bool TreeClass::buildTree()
{
	vector<D3DXMATRIX> bone_transformation_matrices_preloaded;
	TreeInstanceData actual_data;

	D3DXMATRIX translate;
	D3DXMATRIX rotateX;
	D3DXMATRIX rotateY;
	D3DXMATRIX rotateZ;
	D3DXMATRIX scale;
	D3DXMATRIX transform;
	D3DXMatrixTranslation(&translate, 0.0f, 0.0f, 0.0f);
	D3DXMatrixScaling(&scale, 1.0f, 1.0f, 1.0f);
	D3DXMatrixRotationX(&rotateX, 0.0f * 0.0174532925f);
	D3DXMatrixRotationY(&rotateY, 0.0f * 0.0174532925f);
	D3DXMatrixRotationZ(&rotateZ, 0.0f * 0.0174532925f);
	transform = rotateX * rotateZ * rotateY * scale * translate;

	actual_data.offset_matrix = transform;
	D3DXMatrixInverse(&actual_data.offset_matrix_inverse, 0, &actual_data.offset_matrix);
	actual_data.instance_matrix_offset = 0;
	float instance_matrix_offset_counter = m_Components.components[2].model_data->GetBoneCount();
	root_node = newNode(m_Components.components[2].child_count, 0, NULL, 2);
	TreeClass::write_instance_data(&root_node, actual_data);

	for (int i = 0; i < m_Components.components[root_node.modelNum].model_data->GetBoneCount(); i++)
	{
		bone_trans_matrix.push_back(root_node.instance_data.offset_matrix);
		bone_transformation_matrices_preloaded.push_back(D3DXMATRIX(
									1.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f, 0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f));
	}
	root_node.instance_data.bone_transformation_matrices = bone_transformation_matrices_preloaded;
	
	m_Components.components[2].instance_count++;
	all_instance_count++;

	TreeClass::TreeNode* new_node;
	TreeClass::TreeNode* node_iterator = &root_node;
	queue<TreeClass::TreeNode*> queue_current, queue_next;
	
	/* initialize random seed: */
	srand (time(NULL));
	int chosen_component = 1;
	for (int i = 1; i < 400; i++)
		{
			if (node_iterator->child_nodes.size() == node_iterator->max_child_count)
			{
				if (queue_current.size() == 0)
				{
					queue_current = queue_next;
					queue<TreeClass::TreeNode*>().swap(queue_next);
				}
				node_iterator = queue_current.front();
				queue_current.pop();
			}

			if (node_iterator->child_nodes.size() != node_iterator->max_child_count)
			{
				// data operations on new node data
				bone_transformation_matrices_preloaded.clear();
				for (int j = 0; j < m_Components.components[chosen_component].model_data->GetBoneCount(); j++)
				{
					bone_transformation_matrices_preloaded.push_back(D3DXMATRIX(
						1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f));
				}
				actual_data.bone_transformation_matrices = bone_transformation_matrices_preloaded;
				actual_data.offset_matrix = m_Components.components[node_iterator->modelNum].child_transforms[node_iterator->child_nodes.size()] * read_instance_data(node_iterator).offset_matrix;
				D3DXMatrixInverse(&actual_data.offset_matrix_inverse, 0, &actual_data.offset_matrix);
				actual_data.instance_matrix_offset = instance_matrix_offset_counter;
				instance_matrix_offset_counter += m_Components.components[chosen_component].model_data->GetBoneCount();
				m_Components.components[chosen_component].instance_count++;
				all_instance_count++;

				new_node = new TreeClass::TreeNode();
				*new_node = TreeClass::newNode(m_Components.components[chosen_component].child_count, node_iterator->child_nodes.size() + 1, node_iterator, chosen_component);
				TreeClass::write_instance_data(new_node, actual_data);
				TreeClass::append_node(node_iterator, new_node);
				queue_next.push(new_node);
				for (int j = 0; j < m_Components.components[new_node->modelNum].model_data->GetBoneCount(); j++)
				{
					bone_trans_matrix.push_back(new_node->instance_data.offset_matrix);
				}
			}
			chosen_component = rand() % 2;
		}

	chosen_component = 3;
	while (true) {
		if (node_iterator->child_nodes.size() != node_iterator->max_child_count)
		{
			//////
			bone_transformation_matrices_preloaded.clear();
			for (int j = 0; j < m_Components.components[chosen_component].model_data->GetBoneCount(); j++)
			{
				bone_transformation_matrices_preloaded.push_back(D3DXMATRIX(
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f));
			}
			actual_data.bone_transformation_matrices = bone_transformation_matrices_preloaded;
			actual_data.offset_matrix = m_Components.components[node_iterator->modelNum].child_transforms[node_iterator->child_nodes.size()] * read_instance_data(node_iterator).offset_matrix;
			D3DXMatrixInverse(&actual_data.offset_matrix_inverse, 0, &actual_data.offset_matrix);
			actual_data.instance_matrix_offset = instance_matrix_offset_counter;
			instance_matrix_offset_counter += m_Components.components[chosen_component].model_data->GetBoneCount();
			m_Components.components[chosen_component].instance_count++;
			all_instance_count++;

			new_node = new TreeClass::TreeNode();
			*new_node = TreeClass::newNode(m_Components.components[chosen_component].child_count, node_iterator->child_nodes.size() + 1, node_iterator, chosen_component);
			TreeClass::write_instance_data(new_node, actual_data);
			TreeClass::append_node(node_iterator, new_node);
			for (int j = 0; j < m_Components.components[new_node->modelNum].model_data->GetBoneCount(); j++)
			{
				bone_trans_matrix.push_back(new_node->instance_data.offset_matrix);
			}
		}
		////////
		if (node_iterator->child_nodes.size() == node_iterator->max_child_count)
		{
			if (queue_current.size() == 0)
			{
				if (queue_next.size() == 0)
				{
					break;
				} else {
					queue_current = queue_next;
					queue<TreeClass::TreeNode*>().swap(queue_next);
				}
			}
			node_iterator = queue_current.front();
			queue_current.pop();
		}
	}

	for (int i = 0; i < m_Components.components.size(); ++i) {
		m_Components.components[i].model_data->setInstanceCount(m_Components.components[i].instance_count);
	}

	return true;
}

ModelClass* TreeClass::GetModel(unsigned int modelNum)
{
	return m_Components.components[modelNum].model_data;
}

int TreeClass::GetInstanceCount(unsigned int modelNum)
{
	return m_Components.components[modelNum].instance_count;
}

int TreeClass::GetAllInstanceCount()
{
	return all_instance_count;
}