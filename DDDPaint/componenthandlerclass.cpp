////////////////////////////////////////////////////////////////////////////////
// Filename: componenthandlerclass.cpp
////////////////////////////////////////////////////////////////////////////////

#include "componenthandlerclass.h"

ComponentHandlerClass::ComponentHandlerClass()
{
}

ComponentHandlerClass::ComponentHandlerClass(const ComponentHandlerClass& other)
{
}

ComponentHandlerClass::~ComponentHandlerClass()
{
}

bool ComponentHandlerClass::Initialize(char* compFilePath)
{
	bool result;

	result = ParseComponentsFile(compFilePath);
	
	return result;
}

void ComponentHandlerClass::Shutdown()
{
	// Release the model object.
	components.clear();
	
	return;
}

void ComponentHandlerClass::Render(ID3D11DeviceContext* deviceContext)
{
	return;
}

void ComponentHandlerClass::LoadModel(unsigned int modelNum)
{
	components[modelNum].model_data->LoadModel(components[modelNum].model_path.c_str());
}

void ComponentHandlerClass::InitModel(unsigned int modelNum, ID3D11Device* device, vector<ModelClass::InstanceType> instances)
{
	if (instances.size() > 0) {
		components[modelNum].model_data->Initialize(device, L"../DDDPaint/data/dragon.png", instances);
	}
}

bool ComponentHandlerClass::ParseComponentsFile(string compFilePath) {
	std::ifstream file(compFilePath.c_str());
	std::stringstream is_file;

	components.clear();

	Component tempComponent;

	if (file)
	{
		is_file << file.rdbuf();
		file.close();
	}
	else {
		return false;
	}

	// read in file content and contruct components
	std::string line;
	do {
		tempComponent.child_transforms.clear();

		std::getline(is_file, line);
		tempComponent.name = line;
		std::getline(is_file, line);
		if (line.compare("Stump")) {
			tempComponent.type = STUMP;
		}
		else if (line.compare("Bole")) {
			tempComponent.type = BOLE;
		}
		else if (line.compare("Fork")) {
			tempComponent.type = FORK;
		}
		else if (line.compare("Tip")) {
			tempComponent.type = TIP;
		}
		std::getline(is_file, line);
		tempComponent.child_count = atoi(line.c_str());
		std::getline(is_file, line);
		tempComponent.model_path = line;
		for(int i = 0; i < tempComponent.child_count; ++i) {
			D3DXMATRIX translate;
			D3DXMATRIX rotateX;
			D3DXMATRIX rotateY;
			D3DXMATRIX rotateZ;
			D3DXMATRIX scale;
			D3DXMATRIX transform;

			string temp;
			float temp_x, temp_y, temp_z;

			std::getline(is_file, line);
			std::istringstream is_line(line);
			std::getline(is_line, temp, ' ');
			temp_x = atof(temp.c_str());
			std::getline(is_line, temp, ' ');
			temp_y = atof(temp.c_str());
			std::getline(is_line, temp);
			temp_z = atof(temp.c_str());

			D3DXMatrixTranslation(&translate, temp_x, temp_y, temp_z);

			std::getline(is_file, line);
			is_line = std::istringstream(line);
			std::getline(is_line, temp, ' ');
			temp_x = atof(temp.c_str());
			std::getline(is_line, temp, ' ');
			temp_y = atof(temp.c_str());
			std::getline(is_line, temp);
			temp_z = atof(temp.c_str());

			D3DXMatrixScaling(&scale, temp_x, temp_y, temp_z);

			std::getline(is_file, line);
			is_line = std::istringstream(line);
			std::getline(is_line, temp, ' ');
			temp_x = atof(temp.c_str());
			std::getline(is_line, temp, ' ');
			temp_y = atof(temp.c_str());
			std::getline(is_line, temp);
			temp_z = atof(temp.c_str());

			D3DXMatrixRotationX(&rotateX, temp_x * 0.0174532925f);
			D3DXMatrixRotationY(&rotateY, temp_y * 0.0174532925f);
			D3DXMatrixRotationZ(&rotateZ, temp_z * 0.0174532925f);

			transform = rotateX * rotateZ * rotateY * scale * translate;

			tempComponent.child_transforms.push_back(transform);
		}
		components.push_back(tempComponent);
		std::getline(is_file, line);
	} while (!line.compare("*"));

	return true;
}