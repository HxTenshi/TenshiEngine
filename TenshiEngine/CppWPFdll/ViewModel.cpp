#include "stdafx.h"

#include "ViewModel.h"


template <>
String^ lexical_cast_(float* p){
	return System::Convert::ToString(*p);
}
template <>
String^ lexical_cast_(int* p){
	return System::Convert::ToString(*p);
}
template <>
String^ lexical_cast_(double* p){
	return System::Convert::ToString(*p);
}
template <>
String^ lexical_cast_(bool* p){
	return System::Convert::ToString(*p);
}

template <>
String^ lexical_cast_(std::string* p){
	return gcnew String(p->c_str());
}


template <>
float lexical_cast<float, String>(String^ p){
	return System::Convert::ToSingle(p);
}
template <>
double lexical_cast<double, String>(String^ p){
	return System::Convert::ToDouble(p);
}
template <>
int lexical_cast<int, String>(String^ p){
	return System::Convert::ToInt32(p);
}
template <>
bool lexical_cast<bool, String>(String^ p){
	return System::Convert::ToBoolean(p);
}
template <>
std::string lexical_cast<std::string, String>(String^ p){
	pin_ptr<const wchar_t> wch = PtrToStringChars(p);
	size_t convertedChars = 0;
	size_t  sizeInBytes = ((p->Length + 1) * 2);
	char    *ch = (char *)malloc(sizeInBytes);
	wcstombs_s(&convertedChars,
		ch, sizeInBytes,
		wch, sizeInBytes);

	std::string temp(ch);
	free(ch);

	return temp;
}