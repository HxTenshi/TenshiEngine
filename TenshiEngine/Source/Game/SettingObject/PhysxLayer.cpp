#include "PhysxLayer.h"
#include "Window/Window.h"
#include "Physx/Physx3.h"
#include "Game/Game.h"

PhysxLayer::PhysxLayer(){
	mSelects.push_back("None");
	mSelects.push_back("Layer1");
	mSelects.push_back("Layer2");
	mSelects.push_back("Layer3");
	mSelects.push_back("Layer4");
	mSelects.push_back("Layer5");
	mSelects.push_back("Layer6");
	mSelects.push_back("Layer7");
	mSelects.push_back("Layer8");
	mSelects.push_back("Layer9");
	mSelects.push_back("Layer10");
	mSelects.push_back("Layer11");
	mSelects.push_back("Layer12");


	for (int I = 0; I < 13; I++){
		for (int J = I; J < 13; J++){
			int i = 1 << I;
			int j = 1 << J;
			mCollideFiler[i | j] = true;
		}
	}

	for (int I = 0; I < 13; I++){
		for (int J = I; J < 13; J++){
			int i = 1 << I;
			int j = 1 << J;

			SetLayerFlag(I,J, mCollideFiler[i | j]);
		}
	}
}

PhysxLayer::~PhysxLayer(){

}

#ifdef _ENGINE_MODE
void PhysxLayer::CreateInspector(){
	auto data = Window::CreateInspector();

	for (int i = 0; i < 13; i++){
		Window::AddInspector(new TemplateInspectorDataSet<std::string>(std::to_string(i), &mSelects[i], [&,i](std::string f){
			mSelects[i] = f;
		}), data);
	}

	for (int I = 0; I < 13; I++){
		for (int J = I; J < 13; J++){
			int i = 1 << I;
			int j = 1 << J;
			std::string text = std::to_string(I) + "x" + std::to_string(J);

			Window::AddInspector(new TemplateInspectorDataSet<bool>(text, &mCollideFiler[i | j], [&, I, J](bool f){
				
				SetLayerFlag(I, J, f);
			}), data);
		}
	}

	Window::ViewInspector("PhysxLayer", NULL, data, this);


}
#endif


void PhysxLayer::SetLayerFlag(int l1, int l2, bool f){
	int layer1 = (1 << l1);
	int layer2 = (1 << l2);

	mCollideFiler[layer1 | layer2] = f;
	Game::GetPhysX()->SetLayerCollideFlag((Layer::Enum)layer1, (Layer::Enum)layer2, f);
}

void PhysxLayer::_ExportData(I_ioHelper* io, bool childExport){


#define _KEY_COMPEL(x) io->func( x , #x,true)
#define _KEY_COMPEL_ARR(i_,x) io->func( x , (#x + std::to_string(##i_)).c_str(),true)

	for (int i = 0; i < 13; i++){
		_KEY_COMPEL_ARR(i, mSelects[i]);
	}

	for (int I = 0; I < 13; I++){
		for (int J = I; J < 13; J++){
			int i = 1 << I;
			int j = 1 << J;

			int k = i | j;

			_KEY_COMPEL_ARR(k, mCollideFiler[k]);

		}
	}

#undef _KEY
#undef _KEY_COMPEL
}

void PhysxLayer::_ImportData(I_ioHelper* io){


#define _KEY_COMPEL(x) io->func( x , #x)
#define _KEY_COMPEL_ARR(i_,x) io->func( x , (#x + std::to_string(##i_)).c_str())


	for (int i = 0; i < 13; i++){
		_KEY_COMPEL_ARR(i, mSelects[i]);
	}

	for (int I = 0; I < 13; I++){
		for (int J = I; J < 13; J++){
			int i = 1 << I;
			int j = 1 << J;

			int k = i | j;

			_KEY_COMPEL_ARR(k, mCollideFiler[k]);

			SetLayerFlag(I,J, mCollideFiler[i | j]);

		}
	}


#undef _KEY
#undef _KEY_COMPEL
}
