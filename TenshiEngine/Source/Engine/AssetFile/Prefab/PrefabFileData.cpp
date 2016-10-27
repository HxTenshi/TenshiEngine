#include "PrefabFileData.h"


#include "Game/Actor.h"
#include "MySTL/ioHelper.h"

PrefabFileData::PrefabFileData(){

	m_PrefabActor = NULL;
	mBeforeParam = NULL;

}
PrefabFileData::~PrefabFileData(){
	m_PrefabActor = NULL;
	if (mBeforeParam){
		delete mBeforeParam;
		mBeforeParam = NULL;
	}
	m_PrefabActorChildren.clear();
	m_FileName = "";
}

bool PrefabFileData::Create(const char* filename){

	if (!m_PrefabActor){
		m_PrefabActor = make_shared<Actor>();
		m_PrefabActor->SetInspectorFindGameObjectFunc([&](auto id)->wp<Actor> {
			if (m_PrefabActor->GetBeforeUniqueID() == id) {
				return m_PrefabActor;
			}
			for (auto child : m_PrefabActorChildren) {
				if (child->GetBeforeUniqueID() == id) {
					return child;
				}
			}
			return NULL;
		});
	}

	if (mBeforeParam) {
		delete mBeforeParam;
		mBeforeParam = NULL;
	}
	mBeforeParam = new picojson::value();

	m_FileName = filename;
	std::list<shared_ptr<Actor>> list;
	m_PrefabActor->ImportDataAndNewID(m_FileName, [&](shared_ptr<Actor> obj) { list.push_back(obj); });
	m_PrefabActorChildren = list;
	m_PrefabActor->PlayInitializeStageColl();
	m_PrefabActor->ExportData(*mBeforeParam,true);
	return true;

}
picojson::value PrefabFileData::Apply(){

	//auto file = behind_than_find_last_of(m_FileName, "/");
	//file = forward_than_find_last_of(file, ".");
	//auto path = forward_than_find_last_of(m_FileName, "/");

	m_PrefabActor->ExportData(m_FileName, true);

	//picojson::value back;

	//if (mBeforeParam){
	//	back = *mBeforeParam;
	//	delete mBeforeParam;
	//	mBeforeParam = NULL;
	//}

	//mBeforeParam = new picojson::value();
	//m_PrefabActor->ExportData(*mBeforeParam, true);


	return *mBeforeParam;
}

shared_ptr<I_InputHelper> PrefabFileData::GetData() const{
	return make_shared<MemoryInputHelper>(*mBeforeParam,(I_InputHelper*)NULL);
}