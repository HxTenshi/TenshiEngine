#include "ICommand.h"

#include "Library/picojson.h"
#include "Game/Game.h"

ActorUndoCommand::ActorUndoCommand(Actor* actor,bool childExport)
{
	actor->ExportData(mPrev, childExport);
}

void ActorUndoCommand::Undo(){
	auto act = make_shared<Actor>();
	GameObject This = act;

	act->ImportData(mPrev, [](auto o) {
		auto id = o->GetUniqueID();
		
		if (auto tar = Game::FindUID(id)) {
			Game::DestroyObject(tar.lock());
		}
		Game::AddObject(o,false,true); });
	auto id = act->GetUniqueID();
	if (auto tar = Game::FindUID(id)){
		Game::DestroyObject(tar.lock());
	}
	Game::AddObject(act);
}
//void ActorUndoCommand::Undo() {
//	auto target = Game::FindUID(id);
//	if (target) {
//		target->Finish();
//		target->ImportData(mPrev);
//		target->Initialize();
//		target->Start();
//	}
//}

ActorDestroyUndoCommand::ActorDestroyUndoCommand(Actor* actor)
{
	mPrev = actor->GetUniqueID();
}

void ActorDestroyUndoCommand::Undo(){
	if (auto tar = Game::FindUID(mPrev)){
		Game::DestroyObject(tar.lock());
	}
}

LinkUndoCommand::LinkUndoCommand(std::list<ICommand*>* list)
{
	mPrev = list;
}

LinkUndoCommand::~LinkUndoCommand(){

	for (auto com : *mPrev){
		delete com;
	}
	delete mPrev;
}
void LinkUndoCommand::Undo(){
	for (auto c : *mPrev){
		c->Undo();
	}
}
