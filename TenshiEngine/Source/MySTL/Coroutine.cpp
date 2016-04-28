#include "Coroutine.h"

#include "Window/Window.h"
class Coroutine{
public:

	Coroutine(const std::function<void(void)>& coroutine){
		mDestroy = false;
		mCoroutineThread = NULL;
		mCoroutineThreadID = NULL;
		mCoroutineState = CoroutineState::Standby;

		mCoroutine = coroutine;
	}
	~Coroutine(){
		Release();
	}

	void StartCoroutine(){
		if (mCoroutineThread == NULL){
			CreateCoroutineThread();
		}
	}

	void Tick(){
		if (mCoroutineThread){
			mCoroutineState = CoroutineState::Play;
			while (!mDestroy){
				if (mCoroutineState == CoroutineState::Wait || mCoroutineState == CoroutineState::Complete)break;
			}
			if (mCoroutineState == CoroutineState::Complete){
				Release();
			}
		}
	}

	bool IsComplete() const{
		return mCoroutineState == CoroutineState::Complete;
	}

private:


	void Release(){
		mDestroy = true;

		if (mCoroutineThread){

			if (mCoroutineState == CoroutineState::Complete){

				CloseHandle(mCoroutineThread);
				mCoroutineThread = NULL;
			}
		}
	}

	int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {

		if (mDestroy){
			return EXCEPTION_EXECUTE_HANDLER;
		}

		mCoroutineState = CoroutineState::Wait;
		while (!mDestroy){
			if (mCoroutineState == CoroutineState::Play)break;
		}
		if (mDestroy){
			return EXCEPTION_EXECUTE_HANDLER;
		}

		EXCEPTION_RECORD *xr = ep->ExceptionRecord;
		auto *xc = ep->ContextRecord;
		// 1 つずつ進める（x86 上では 1 バイト）
		++xc->Eip;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	static int ThreadProc(void* scr){
		auto This = (Coroutine*)scr;
		__try{
			while (!This->mDestroy){
				if (This->mCoroutineState == CoroutineState::Play)break;
			}
			if (!This->mDestroy){
				This->mCoroutine();
				//func();
			}
		}
		__except (This->filter(GetExceptionCode(), GetExceptionInformation())){

		}
		This->mCoroutineState = CoroutineState::Complete;
		return 0;
	}

	void CreateCoroutineThread(){
		mCoroutineThread = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)Coroutine::ThreadProc,
			(LPVOID)this, 0, &mCoroutineThreadID);
	}



	volatile bool mDestroy;

	enum class CoroutineState{
		Standby,
		Play,
		Wait,
		Complete,
	};
	volatile CoroutineState mCoroutineState;

	HANDLE mCoroutineThread;
	DWORD mCoroutineThreadID;
	std::function<void(void)> mCoroutine;


};









CoroutineSystem::CoroutineSystem(){
}

CoroutineSystem::~CoroutineSystem(){
	mCoroutines.remove_if([](const Coroutine* p){
		delete p;
		return true;
	});
}

void CoroutineSystem::StartCoroutine(const std::function<void(void)>& coroutine){
	auto cor = new Coroutine(coroutine);
	cor->StartCoroutine();
	mCoroutines.push_back(cor);
}

void CoroutineSystem::Tick(){

	for (auto cor : mCoroutines){
		cor->Tick();
	}

	mCoroutines.remove_if([](const Coroutine* p){
		bool f = p->IsComplete(); 
		if (f)delete p;
		return f;
	});
}

int CoroutineSystem::ActiveCoroutineCount(){
	return (int)mCoroutines.size();
}