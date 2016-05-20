#include "Script.h"
//#include "Game\Game.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/PhysXComponent.h"
#include "Game/Component/TextComponent.h"
#include "Input/Input.h"
#include "MySTL/ptr.h"
#include "Game/Script/IGame.h"
#include "Engine\Debug.h"

#include <functional>
#include <map>

//#define INITGUID
#include <dinput.h>

#include "System\include.h"


class FuctorySetter{
public:
	FuctorySetter(){
#define _ADD(x) mFactory[typeid(x).name()] = [](){ return new x(); }
		
		//ここに作成したクラスを追加します

#include "System\factory.h"

#undef _ADD
	}
	std::map<std::string, std::function<IDllScriptComponent*()>> mFactory;
};

FuctorySetter mFact;

IDllScriptComponent* CreateInstance(const char* ClassName){
	std::string name = "class ";
	name += ClassName;
	auto func = mFact.mFactory.find(name);
	if (func == mFact.mFactory.end())return NULL;
	return func->second();
}
void ReleseInstance(IDllScriptComponent* p){
	delete p;
}


#include <bitset>
#include <sstream>
DebugEngine* debug;
int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {

	
	auto address = (unsigned long)ep->ExceptionRecord->ExceptionAddress;
	auto ecode = ep->ExceptionRecord->ExceptionCode;

	{
		std::ostringstream os;
		os << std::hex << address;
		debug->Log(" Exceptエラー Address[" + os.str() + "]");
	}
{
		std::ostringstream os;
		os << std::hex << code;
		debug->Log(" +-- Code[" + os.str() + "]");
	}
	if (code == EXCEPTION_FLT_DIVIDE_BY_ZERO) {
		debug->Log(" +-- : ゼロ除算");
	}
	else if (code == EXCEPTION_INT_DIVIDE_BY_ZERO) {
		debug->Log(" +-- : ゼロ除算");
	}
	else if (code == EXCEPTION_ACCESS_VIOLATION) {
		debug->Log(" +-- : アクセス違反");
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

void Function0(IDllScriptComponent* com, IDllScriptComponent::Func0 func){
	debug = com->game->Debug();
	__try{
		//try{
			(com->*func)();
		//}
		//catch (char* text){
		//	debug->Log(text);
		//}
	}
	__except (filter(GetExceptionCode(), GetExceptionInformation())){

	}
}
void Function1(IDllScriptComponent* com, IDllScriptComponent::Func1 func, Actor* tar){
	debug = com->game->Debug();
	__try{
		//try{
			(com->*func)(tar);
		//}
		//catch (char* text){
		//	debug->Log(text);
		//}

	}
	__except (filter(GetExceptionCode(), GetExceptionInformation())){

	}
}
