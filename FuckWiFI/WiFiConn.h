#pragma once
#include "WiFilog.h"

#include <sstream>
#include <string>
#include <vector>
#include <memory>


#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <stdio.h>
#include <stdlib.h>

class WiFiConn
{
public:
	WiFiConn();
	~WiFiConn();

	bool connect(int interface_index, std::string wifiName = "");
	
	//get wlan interface (hard adapter)
	bool get_interface_info();
	int get_interface_status(std::string& desc);
	int get_interface_status();
	bool scan_wlan_list(int interface_index = 0);
	bool connect(const std::string wifiname, const std::string password);


private:

	PWLAN_INTERFACE_INFO_LIST pwlan_interface_list = nullptr;
	HANDLE wlan_handle = nullptr;

	std::vector<WLAN_AVAILABLE_NETWORK>* entries = nullptr;
	
	//open wlan client handle
	bool open_handle();
	std::shared_ptr<WLAN_CONNECTION_PARAMETERS> build_wlan_parameters(WLAN_AVAILABLE_NETWORK entry, const std::wstring * pwifiname, const std::string& pass);

};

