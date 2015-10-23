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


#include <fstream>
#include <vector>
#include <type_traits>
class fstd{
public:
	template<class T>
	static void r_vector(std::vector<T>& read, FILE *hFP){


		static_assert(std::is_pointer<T>::value == false, "error");

		int num;
		fread(&num, sizeof(int), 1, hFP);
		read.resize(num);
		fread(read.data(), sizeof(T), read.size(), hFP);
	}

	template<class T>
	static void r(T& read, FILE *hFP){

		static_assert(std::is_pointer<T>::value == false, "error");

		fread(&read, sizeof(T), 1, hFP);
	}

	template<>
	static void r(std::string& read, FILE *hFP){

		int num;
		fread(&num, sizeof(int), 1, hFP);

		read.resize(num);

		fread((void*)read.data(), sizeof(const char), num, hFP);
		read += "\0";
	}
};