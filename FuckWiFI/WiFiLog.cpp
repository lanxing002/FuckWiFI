#include "pch.h"
#include "WiFiLog.h"
#include <ctime>
#include <chrono>

WiFiLog::WiFiLog(const std::string path)
{
	_logFile.open(path, std::ofstream::out | std::ofstream::app);
}

WiFiLog::WiFiLog(const char* path) :WiFiLog(std::string(path)) {
}

WiFiLog::WiFiLog() {

}

WiFiLog::~WiFiLog()
{
	//ǿ��ˢ�²��ر�
	_logFile.flush();
	_logFile.close();
}

WiFiLog& WiFiLog::logTime() {

	//auto tt = std::chrono::system_clock::now();
	//std::time_t current = std::chrono::system_clock::to_time_t(tt);

	//// convert now to string form
	// _logFile << std::ctime(&current) << ": ";
	_logFile << "time xxxx: ";

	return *this;
}

WiFiLog& WiFiLog::operator<<(const char* str) {
	_logFile << str;
	return *this;
}

WiFiLog& WiFiLog::operator<<(const std::string& str) {
	_logFile << str;
	return *this;
}

WiFiLog& WiFiLog::operator<<(const int code) {
	_logFile << code;
	return *this;
}

WiFiLog& WiFiLog::operator<<(const WCHAR* wstr) {
	_bstr_t b(wstr);
	
	//�Զ�����ת��
	_logFile << b;
	return *this;
}