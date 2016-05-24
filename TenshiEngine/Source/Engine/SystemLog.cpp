#include"SystemLog.h"
#include "MySTL/File.h"
#include <iostream>
#include <time.h>

const std::string LogFilePath = "Temp/SystemLog.txt";

int inc = 0;

void SystemLog::Create(){
	File f(LogFilePath);
	f.FileCreate();
}
void SystemLog::Print(const std::string& log){
	std::ofstream f(LogFilePath,std::ios::app | std::ios::out);
	auto c = clock();
	for (int i = 0; i < inc;i++)
		f << "  ";
	f << c << ":" << log << std::endl;

}

SystemLogHelper::SystemLogHelper(const std::string& log)
	:mError(false)
	, mLog(log){

	SystemLog::Print(mLog + "...ŠJŽn");
	inc++;

}
SystemLogHelper::~SystemLogHelper(){

	inc--;
	auto r = mError ? "Ž¸”s" : "I—¹";
	SystemLog::Print(mLog + "..." + r);
}

void SystemLogHelper::Error(){
	mError = true;
}