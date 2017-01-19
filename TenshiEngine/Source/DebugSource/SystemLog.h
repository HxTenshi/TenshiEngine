#pragma once

#include <string>


class SystemLog{
public:

	static void Create();
	static void Print(const std::string& log);

private:
};


class SystemLogHelper{
public:
	SystemLogHelper(const std::string& log);
	~SystemLogHelper();

	void Error();
private:
	bool mError;
	std::string mLog;
};

#define _FILE (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define _SYSTEM_LOG_ERROR(x) SystemLog::Print(std::string("")+ x + "...Ž¸”s [" +_FILE+","+std::to_string(__LINE__)+"]")
#define _SYSTEM_LOG(x) SystemLog::Print(x)
#define _SYSTEM_LOG_H(x) SystemLogHelper __slh__(x)
#define _SYSTEM_LOG_H_ERROR() __slh__.Error()