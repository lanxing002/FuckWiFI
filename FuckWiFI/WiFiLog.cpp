#include "pch.h"
#include "WiFiLog.h"
#include <ctime>
#include <chrono>

WiFiLog::WiFiLog(const std::string path)
{
	retract = 0;
	_logFile.open(path, std::ofstream::out | std::ofstream::app);
}

WiFiLog::WiFiLog(const char* path) :WiFiLog(std::string(path)) {
}

WiFiLog::WiFiLog() {

}

WiFiLog::~WiFiLog()
{
	//强制刷新并关闭
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


int WiFiLog::out_retract() {
	for (int i = 0; i < retract; i++)
		_logFile << "\t";
	return 0;
}

WiFiLog& WiFiLog::operator<<(const char* str) {
	out_retract();
	_logFile << str;
	return *this;
}

WiFiLog& WiFiLog::operator<<(const std::string& str) {
	out_retract();
	_logFile << str;
	return *this;
}

WiFiLog& WiFiLog::operator<<(const int code) {
	_logFile << code;
	return *this;
}

WiFiLog& WiFiLog::operator<<(char ch) {
	_logFile << ch;
	return *this;
}

WiFiLog& WiFiLog::operator<<(DWORD dword) {
	_logFile << (int)dword;
	return *this;
}

WiFiLog& WiFiLog::operator<<(const WCHAR* wstr) {
	
	out_retract();
	_bstr_t b(wstr);
	
	//自动类型转换
	_logFile << b;
	return *this;
}