#pragma once
#include <windows.h>
class WlanApiErrorWrapper
{
public:
	//wraps OpenHandle method errors
	static bool wrap_open_handle_result(const DWORD);
	
	//wrap enum_interface result
	static bool wrap_enum_interface_result(const DWORD);
	
	//wrap get network list 
	static bool wrap_get_network_list_result(const DWORD);
	
	//wrap set profile result
	static bool wrap_set_profile_result(const DWORD, const DWORD);
	
	//wrap connect result
	static bool wrap_connect_result(const DWORD);

	//wraps WlanRegisterNotification result
	static bool wrap_register_notification_result(const DWORD);
};

