#include "PrefabFileData.h"


#include "Game/Actor.h"
#include "MySTL/ioHelper.h"

PrefabFileData::PrefabFileData(){

	m_PrefabActor = NULL;
	mBeforeParam = NULL;

}
PrefabFileData::~PrefabFileData(){
	if (m_PrefabActor){
		delete m_PrefabActor;
		m_PrefabActor = NULL;
	}
	if (mBeforeParam){
		delete mBeforeParam;
		mBeforeParam = NULL;
	}
	mFileName = "";
}

void PrefabFileData::Create(const char* filename){


	if (m_PrefabActor){
		delete m_PrefabActor;
		m_PrefabActor = NULL;
	}
	if (mBeforeParam){
		delete mBeforeParam;
		mBeforeParam = NULL;
	}

	mFileName = filename;


	m_PrefabActor = new Actor();
	m_PrefabActor->ImportData(mFileName);

	mBeforeParam = new picojson::value();
	m_PrefabActor->ExportData(*mBeforeParam);

}
void PrefabFileData::FileUpdate(const char* filename){

	mFileName = filename;

	m_PrefabActor->ImportData(mFileName);

}
picojson::value PrefabFileData::Apply(){

	auto file = behind_than_find_last_of(mFileName, "/");
	file = forward_than_find_last_of(file, ".");
	auto path = forward_than_find_last_of(mFileName, "/");

	m_PrefabActor->ExportData(path, file);

	picojson::value back;

	if (mBeforeParam){
		back = *mBeforeParam;
		delete mBeforeParam;
		mBeforeParam = NULL;
	}

	mBeforeParam = new picojson::value();
	m_PrefabActor->ExportData(*mBeforeParam);


	return back;
}

shared_ptr<I_InputHelper> PrefabFileData::GetData() const{
	return make_shared<MemoryInputHelper>(*mBeforeParam,(I_InputHelper*)NULL);
}