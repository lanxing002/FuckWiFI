#pragma once
#include <string>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <comdef.h>
/**如何实现成为一个单例类
*/

class WiFiLog
{
public:
	WiFiLog(const std::string path);
	WiFiLog(const char *path);
	WiFiLog();
	~WiFiLog();

	//输出日志
	WiFiLog& logTime();

	WiFiLog& operator<<(const std::string& str);
	WiFiLog& operator<<(const char* str);
	WiFiLog& operator<<(int code);
	WiFiLog& operator<<(const WCHAR* wstr);

private:
	std::ofstream _logFile;
};
