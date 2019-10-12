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
	WiFiLog& operator<<(char ch);
	WiFiLog& operator<<(DWORD dword);

	void retract_reset(unsigned int num = 0) { retract = num; };
	void plus_retract() { retract++; }
	void sub_retract() { retract = retract > 0 ? retract - 1 : 0; }


private:
	int out_retract();
	std::ofstream _logFile;
	unsigned int retract;
};
