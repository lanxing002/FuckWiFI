#include "pch.h"
#include "WiFiLog.h"


WiFiLog::WiFiLog(const std::string path)
{
	logFile.open(path, std::ofstream::out | std::ofstream::app);
}

WiFiLog::WiFiLog(const char *path) {
	WiFiLog(std::string(path));
}

WiFiLog::~WiFiLog()
{
	//强制刷新并关闭
	logFile.flush();
	logFile.close();
}
