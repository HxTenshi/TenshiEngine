#include "ICommand.h"

#include "Library/picojson.h"
#include "Game/Game.h"

ActorUndoCommand::ActorUndoCommand(Actor* actor)
{
	actor->ExportData(mPrev, true);
}

void ActorUndoCommand::Undo(){
	auto act = new Actor();
	act->ImportData(mPrev);
	auto id = act->GetUniqueID();
	if (auto tar = Game::FindUID(id)){
		Game::DestroyObject(tar);
	}
	Game::AddObject(act);
}

ActorDestroyUndoCommand::ActorDestroyUndoCommand(Actor* actor)
{
	mPrev = actor->GetUniqueID();
}

void ActorDestroyUndoCommand::Undo(){
	if (auto tar = Game::FindUID(mPrev)){
		Game::DestroyObject(tar);
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
