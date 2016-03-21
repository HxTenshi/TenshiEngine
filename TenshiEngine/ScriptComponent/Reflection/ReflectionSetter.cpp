
#include "ReflectionSetter.h"

#define private public
#include "../Script.h"
#undef private

//static
decltype(ReflectionSetter::map) ReflectionSetter::map;

ReflectionSetter reflection;


ReflectionSetter::ReflectionSetter(){
#define REF(PostType,menber) create(#PostType, #menber, typeid(decltype(PostType::menber)).name(), [](void* post){ return (void*)&((PostType*)post)->menber;}, [](void* post, void* Value){ ((PostType*)post)->menber = *(decltype(PostType::menber)*)Value;} )

	//ここにコンポーネントで表示したい変数を追加します

	REF(CameraScript, mRotateY);
	REF(CameraScript, mLength);
	REF(CameraScript, mClear);

#undef REF
}



std::map<std::string, std::map<std::string, MemberInfo_Data>>* GetReflectionData(){
	return reflection.GetReflectionDatas();
}