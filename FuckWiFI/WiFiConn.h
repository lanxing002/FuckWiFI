#pragma once
#include "WiFilog.h"

#include <sstream>
#include <string>

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <stdio.h>
#include <stdlib.h>

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

class WiFiConn
{
public:
	WiFiConn(WiFiLog* log, const std::string wifiname);
	~WiFiConn();

	bool connect(int interface_index, std::string wifiName = "");
	
	int enumInterface();
	int getAvaiableNet(int interface_index = 0);
	int connect();

	bool is_target(const char* str, int length);

	PWLAN_CONNECTION_PARAMETERS build_wlan_parameters(WLAN_AVAILABLE_NETWORK entry, const char* ssid, const char* pass);
private:
	WiFiLog* _log;
	PWLAN_INTERFACE_INFO_LIST _pIfList;
	HANDLE _hClient;
	std::string _wifiname;
	unsigned int retract;
};

