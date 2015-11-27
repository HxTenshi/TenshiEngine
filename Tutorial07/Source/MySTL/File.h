#pragma once

#include <string>
#include <fstream>
#include <Windows.h>

class File{
public:
	File( const std::string& FileName)
	{
		mSpace[0] = '\0';
		Open(FileName);
	}
	File()
	{
		mSpace[0] = '\0';
	}
	~File(){
		mFilePtr << std::flush;
		mFilePtr.close();
	}

	bool Open(const std::string& FileName){

		mFilePtr.close();
		mFileName = FileName;
		mFilePtr.open(mFileName, std::ios::out | std::ios::in);
		mOpen = !mFilePtr.fail();
		int by = 0;//std::ios::binary;
		bool b = (std::ios::binary & by) != 0;
		if (!b){
			mSpace[0] = ' ';
			mSpace[1] = '\0';
		}
		return mOpen;
	}

	bool FileCreate(){
		mFilePtr.close();
		mFilePtr.open(mFileName, std::ios::out | std::ios::in | std::ios::trunc);
		mOpen = !mFilePtr.fail();

		int by = 0;//std::ios::binary;
		bool b = (std::ios::binary & by) != 0;
		if (!b){
			mSpace[0] = ' ';
			mSpace[1] = '\0';
		}
		return mOpen;
	}

	void FileDelete(){
		mFilePtr.close();
		DeleteFile(mFileName.c_str());
	}

	void Seek(int p){
		mFilePtr.seekp(p);
	}

	void Clear(){
		mFilePtr.close();
		mFilePtr.open(mFileName, std::ios::out | std::ios::in | std::ios::trunc);
		mOpen = !mFilePtr.fail();
	}
	template<class T>
	bool Out(const T& data){
		if (!mOpen)return false;
		mFilePtr << data << mSpace;
		return !mFilePtr.fail();
	}

	template<class T>
	bool In(T* out){
		if (!mOpen)return false;
		mFilePtr >> *out;
		return !mFilePtr.fail();
	}
	operator bool(){
		return !mFilePtr.eof() & mOpen;
	}

private:
	std::string mFileName;
	std::fstream mFilePtr;
	bool mOpen;
	char mSpace[2];
};