#pragma once


//class ICommand{
//public:
//	virtual ~ICommand(){}
//	virtual void Undo()=0;
//	virtual void Redo()=0;
//};
//
//template<class ParamT>
//class ChangeParamCommand : public ICommand{
//public:
//	ChangeParamCommand(const ParamT& prev,const ParamT& next,ParamT* target)
//		: mPrev(prev)
//		, mNext(prev)
//		, mTarget(target)
//	{
//	}
//
//	void Undo() override{
//		*mTarget = mPrev;
//	}
//	void Redo() override{
//		*mTarget = mNext;
//	}
//private:
//	ParamT mPrev;
//	ParamT mNext;
//	ParamT* mTarget;
//};
//
//#include <functional>
//template<class ParamT>
//class ChangeParamFuncCommand : public ICommand{
//public:
//	ChangeParamFuncCommand(const ParamT& prev, const ParamT& next, const std::function<void(const ParamT&)>& target)
//		: mPrev(prev)
//		, mNext(next)
//		, mTarget(target)
//	{
//	}
//
//	void Undo() override{
//		(mTarget)(mPrev);
//	}
//	void Redo() override{
//		(mTarget)(mNext);
//	}
//private:
//	ParamT mPrev;
//	ParamT mNext;
//	std::function<void(const ParamT&)> mTarget;
//};

#include "Library/picojson.h"

class ICommand{
public:
	virtual ~ICommand(){}
	virtual void Undo() = 0;
};

class Actor;
class ActorUndoCommand : public ICommand{
public:
	ActorUndoCommand(Actor* actor);
	void Undo() override;
private:
	picojson::value mPrev;
};
class ActorDestroyUndoCommand : public ICommand{
public:
	ActorDestroyUndoCommand(Actor* actor);
	void Undo() override;
private:
	UINT mPrev;
};
#include <list>
class LinkUndoCommand : public ICommand{
public:
	LinkUndoCommand(std::list<ICommand*>* list);
	~LinkUndoCommand();
	void Undo() override;
private:
	std::list<ICommand*>* mPrev;
};


#include <stack>
class CommandManager{
public:
	~CommandManager(){
		while (!mUndoStack.empty()){
			auto c = mUndoStack.top();
			delete c;
			mUndoStack.pop();
		}
		while (!mRedoStack.empty()){
			auto c = mRedoStack.top();
			delete c;
			mRedoStack.pop();
		}
	}
	void SetUndo(ICommand* command){
		mUndoStack.push(command);
		while (!mRedoStack.empty()){
			auto c = mRedoStack.top();
			delete c;
			mRedoStack.pop();
		}
	}
	void SetUndo(Actor* actor){
		mUndoStack.push(new ActorUndoCommand(actor));
		while (!mRedoStack.empty()){
			auto c = mRedoStack.top();
			delete c;
			mRedoStack.pop();
		}
	}

	void Undo(){
		if (mUndoStack.size()<=1)return;
		auto command = mUndoStack.top();
		mRedoStack.push(command);
		mUndoStack.pop();

		command = mUndoStack.top();
		command->Undo();
		
	}
	void Redo(){
		if (mRedoStack.empty())return;
		auto command = mRedoStack.top();
		mRedoStack.pop();
		command->Undo();
		mUndoStack.push(command);
	}

	void Clear(){
		while (!mUndoStack.empty()){
			auto c = mUndoStack.top();
			delete c;
			mUndoStack.pop();
		}
		while (!mRedoStack.empty()){
			auto c = mRedoStack.top();
			delete c;
			mRedoStack.pop();
		}
	}
private:
	//std::stack<ICommand*> mUndoStack;
	//std::stack<ICommand*> mRedoStack;


	std::stack<ICommand*> mUndoStack;
	std::stack<ICommand*> mRedoStack;

};