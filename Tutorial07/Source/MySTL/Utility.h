#pragma once

#include <string>

//最後に見つかった検索位置より前を取り出す
std::string forward_than_find_last_of(const std::string& str, const std::string& findText);
//最後に見つかった検索位置より後ろを取り出す
std::string behind_than_find_last_of(const std::string& str, const std::string& findText);
//最初に見つかった検索位置より前を取り出す
std::string forward_than_find_first_of(const std::string& str, const std::string& findText);
//最初に見つかった検索位置より後ろを取り出す
std::string behind_than_find_first_of(const std::string& str, const std::string& findText);