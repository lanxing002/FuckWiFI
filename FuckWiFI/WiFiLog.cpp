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
	//ǿ��ˢ�²��ر�
	logFile.flush();
	logFile.close();
}
