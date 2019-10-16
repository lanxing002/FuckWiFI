#include "pch.h"
#include "StringHelper.h"

bool StringHelper::is_equal(UCHAR* lhs, const char* rhs) {
	//reinterpret_cast 常用于低级转换
	return strcmp(reinterpret_cast<char*>(lhs), rhs) == 0;
}

std::wstring * StringHelper::string_convert_wstring(const std::string& str) {
	return new std::wstring(std::begin(str), std::end(str));
}

