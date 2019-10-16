#pragma once
#include <string>
#include <wtypes.h>
class StringHelper
{
public :

	//comparers
	static bool is_equal(UCHAR* lhs, const char* rhs);
	static std::wstring * string_convert_wstring(const std::string& str);
};

