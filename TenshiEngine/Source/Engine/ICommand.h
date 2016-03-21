#pragma once


class ICommand{
public:
	virtual ~ICommand(){}
	virtual void Undo()=0;
	virtual void Redo()=0;
};

template<class ParamT>
class ChangeParamCommand : public ICommand{
public:
	ChangeParamCommand(const ParamT& prev,const ParamT& next,ParamT* target)
		: mPrev(prev)
		, mNext(prev)
		, mTarget(target)
	{
	}

	void Undo() override{
		*mTarget = mPrev;
	}
	void Redo() override{
		*mTarget = mNext;
	}
private:
	ParamT mPrev;
	ParamT mNext;
	ParamT* mTarget;
};

#include <functional>
template<class ParamT>
class ChangeParamFuncCommand : public ICommand{
public:
	ChangeParamFuncCommand(const ParamT& prev, const ParamT& next, const std::function<void(const ParamT&)>& target)
		: mPrev(prev)
		, mNext(next)
		, mTarget(target)
	{
	}

	void Undo() override{
		(mTarget)(mPrev);
	}
	void Redo() override{
		(mTarget)(mNext);
	}
private:
	ParamT mPrev;
	ParamT mNext;
	std::function<void(const ParamT&)> mTarget;
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

	void Undo(){
		if (mUndoStack.empty())return;
		auto command = mUndoStack.top();
		mUndoStack.pop();
		command->Undo();
		mRedoStack.push(command);

		//{
		//	if (!mRedoStack.empty()){
		//		auto command = mRedoStack.top();
		//		command->Redo();
		//		if (mUndoStack.empty())return;
		//		command = mUndoStack.top();
		//		mUndoStack.pop();
		//		mRedoStack.push(command);
		//	}
		//	else
		//	{
		//		if (mUndoStack.empty())return;
		//		auto command = mUndoStack.top();
		//		mUndoStack.pop();
		//		command->Undo();
		//		mRedoStack.push(command);
		//	}
		//}

		
	}
	void Redo(){
		if (mRedoStack.empty())return;
		auto command = mRedoStack.top();
		mRedoStack.pop();
		command->Redo();
		mUndoStack.push(command);
	}
private:
	std::stack<ICommand*> mUndoStack;
	std::stack<ICommand*> mRedoStack;

};