#include "Utility.h"

//最後に見つかった検索位置より前を取り出す
std::string forward_than_find_last_of(const std::string& str, const std::string& findText){
	auto index = str.find_last_of(findText);
	if (index == str.npos){
		return "";
	}
	return str.substr(0, index);
}
//最後に見つかった検索位置より後ろを取り出す
std::string behind_than_find_last_of(const std::string& str, const std::string& findText){
	auto index = str.find_last_of(findText);
	if (index == str.npos){
		return "";
	}
	return str.substr(index + findText.length());
}
//最初に見つかった検索位置より前を取り出す
std::string forward_than_find_first_of(const std::string& str, const std::string& findText){
	auto index = str.find_first_of(findText);
	if (index == str.npos){
		return "";
	}
	return str.substr(0, index);
}
//最初に見つかった検索位置より後ろを取り出す
std::string behind_than_find_first_of(const std::string& str, const std::string& findText){
	auto index = str.find_first_of(findText);
	if (index == str.npos){
		return "";
	}
	return str.substr(index + findText.length());
}