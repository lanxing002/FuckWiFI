#pragma once
#include <string>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <comdef.h>
/**���ʵ�ֳ�Ϊһ��������
*/

class WiFiLog
{
public:
	WiFiLog(const std::string path);
	WiFiLog(const char *path);
	WiFiLog();
	~WiFiLog();

	//�����־
	WiFiLog& logTime();

	WiFiLog& operator<<(const std::string& str);
	WiFiLog& operator<<(const char* str);
	WiFiLog& operator<<(int code);
	WiFiLog& operator<<(const WCHAR* wstr);

private:
	std::ofstream _logFile;
};
