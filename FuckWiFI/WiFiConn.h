#pragma once
#include "WiFilog.h"

#include <sstream>

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
	WiFiConn(WiFiLog *log):_log(log){
		_hClient = NULL;
		//_pIfList = NULL;
	}
	~WiFiConn();

	bool connect(int interface_index, std::string wifiName = "");
	
	int enumInterface();
	int getAvaiableNet(int interface_index = 0);
	int connect();
private:
	WiFiLog* _log;
	PWLAN_INTERFACE_INFO_LIST _pIfList;
	HANDLE _hClient;
	unsigned int retract;
};

