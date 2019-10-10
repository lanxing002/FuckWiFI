#pragma once
#include <string>
#include <fstream>
#include <ctime>
/**如何实现成为一个单例类
*/





class WiFiLog
{
public:
	WiFiLog(const std::string path);
	WiFiLog(const char* path);
	~WiFiLog();

	//输出日志
	friend WiFiLog& operator<<(WiFiLog& log, const char* str);
	friend WiFiLog& operator<<(WiFiLog& log, const std::string& str);

private:
	std::ofstream logFile;
};

WiFiLog& operator<<(WiFiLog& log, const char* str) {
	log.logFile << str;
	return log;
}

WiFiLog& operator<<(WiFiLog& log, const std::string& str) {
	log.logFile << str;
	return log;
}