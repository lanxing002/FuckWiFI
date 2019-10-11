#pragma once
#include "WiFilog.h"
class WiFiConn
{
public:
	WiFiConn(WiFiLog *log):_log(log){
		_hClient = NULL;
		//_pIfList = NULL;
	}
	~WiFiConn();

	bool connect(std::string wifiName, int interface_index);
	
	int enumInterface();
	int getAvaiableNet();
	int connect();
private:
	WiFiLog* _log;
	//WLAN_INTERFACE_INFO_LIST _pIfList;
	HANDLE _hClient;

};

