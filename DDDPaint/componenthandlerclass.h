////////////////////////////////////////////////////////////////////////////////
// Filename: componenthandlerclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _COMPONENTHANDLERCLASS_H_
#define _COMPONENTHANDLERCLASS_H_

#include <d3d11.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include "modelclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: ComponentHandlerClass
////////////////////////////////////////////////////////////////////////////////
class ComponentHandlerClass
{
public:
	ComponentHandlerClass();
	ComponentHandlerClass(const ComponentHandlerClass&);
	~ComponentHandlerClass();

	bool Initialize(char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	void LoadModel(unsigned int);
	void InitModel(unsigned int, ID3D11Device*, vector<ModelClass::InstanceType>);

	enum ComponentTypes {
		STUMP,
		BOLE,
		FORK,
		TIP
	};

	struct Component {
		ModelClass* model_data;
		string name;
		ComponentTypes type;
		int child_count;
		string model_path;
		vector<D3DXMATRIX> child_transforms;
		unsigned int instance_count;

		Component() {
			model_data = NULL;
			instance_count = 0;
		}

		~Component() {
			if (model_data) {
				delete model_data;
			}
		}
	};

private:
	bool ParseComponentsFile(string);

public:
	vector<Component> components;
};

#endif