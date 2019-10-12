#include "pch.h"
#include "WiFiConn.h"

#include <iostream>

WiFiConn::~WiFiConn() {
	std::cout << "wifi auto connection end";
	if (_pIfList != NULL) {
		WlanFreeMemory(_pIfList);
		_pIfList = NULL;
	}
}

bool WiFiConn::connect(int interface_index, std::string wifiName) {
	static const WCHAR targetNetWork[] = { 's', 'w', 'u', '-', 'w', 'i', 'f', 'i', '\0' };
	return true;
}

int WiFiConn::enumInterface() {
	//Declare and initialize variables.
	DWORD dwMaxClient = 2;   //    
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	int iRet = 0;
	WCHAR GuidString[40] = { 0 };


	//从服务器获取一个句柄
	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &_hClient);
	if (dwResult != ERROR_SUCCESS) {
		_log->logTime();
		*_log << "WlanOpenHandle failed with error: " << (int)dwResult << "\n";
		// FormatMessage can be used to find out why the function failed
		return 1;
	}

	dwResult = WlanEnumInterfaces(_hClient, NULL, &_pIfList);
	if (dwResult != ERROR_SUCCESS) {
		_log->logTime();
		*_log << "WlanEnumInterfaces failed with error: " << dwResult << "\n";
		// FormatMessage can be used to find out why the function failed
		return 1;
	}
	else {
		_log->logTime();
		*_log << "WlanEnumInterfaces Success: " << dwResult << "\n";
		_log->plus_retract();
		*_log << "Num Entries: " << _pIfList->dwNumberOfItems << "\n";
		*_log << "Current Index: " << _pIfList->dwIndex << "\n";

		PWLAN_INTERFACE_INFO pIfInfo = NULL;

		for (int i = 0; i < (int)_pIfList->dwNumberOfItems; i++) {
			pIfInfo = (WLAN_INTERFACE_INFO *)&_pIfList->InterfaceInfo[i];
			_log->plus_retract();
			*_log << "Interface Index[" << i << "]\n";
			iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString, 39);
			// For c rather than C++ source code, the above line needs to be
			// iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39);
			if (iRet == 0)
				*_log << "StringFromGUID2 failed\n";
			else {
				*_log << "InterfaceGUID: " << GuidString << "\n";
			}

			*_log << "Interface Description[" << i << "]:" << pIfInfo->strInterfaceDescription << "\n";
			*_log << "Interface State[" << i << "]:\t";
			switch (pIfInfo->isState) {
			case wlan_interface_state_not_ready:
				*_log << "Not ready\n";
				break;
			case wlan_interface_state_connected:
				*_log << "Connected\n";
				break;
			case wlan_interface_state_ad_hoc_network_formed:
				*_log << "First node in a ad hoc network\n";
				break;
			case wlan_interface_state_disconnecting:
				*_log << "Disconnecting\n";
				break;
			case wlan_interface_state_disconnected:
				*_log << "Not connected\n";
				break;
			case wlan_interface_state_associating:
				*_log << "Attempting to associate with a network\n";
				break;
			case wlan_interface_state_discovering:
				*_log << "Auto configuration is discovering settings for the network\n";
				break;
			case wlan_interface_state_authenticating:
				*_log << "In process of authenticating\n";
				break;
			default:
				*_log << "Unknown state %ld\n", pIfInfo->isState;
				break;
			}
			_log->retract_reset();
			*_log << "\n\n\n";
		}
	}
	return 0;
}


int WiFiConn::getAvaiableNet(int interface_index) {
	
	//Declare and initialize variables.
	DWORD dwResult = 0;
	int iRet = 0;
	PWLAN_AVAILABLE_NETWORK_LIST pBssList;
	//默认使用第一张网卡
	PWLAN_INTERFACE_INFO pIfInfo = NULL;
	pIfInfo = (WLAN_INTERFACE_INFO*)& _pIfList->InterfaceInfo[interface_index];

	//reset the log retract
	_log->retract_reset();

	dwResult = WlanGetAvailableNetworkList(_hClient, &(pIfInfo->InterfaceGuid),
		WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES,
		NULL,
		&pBssList);
	_log->logTime();
	*_log << "start scan Network ... ...\n";
	_log->plus_retract();

	if (dwResult != ERROR_SUCCESS) {
		*_log << "Wlan Scan Network failed with error:" << dwResult << "\n\n\n";
		return 1;
	}
	else {
		*_log << "Num Entries: " <<  pBssList->dwNumberOfItems << "\n";
		_log->plus_retract();
		PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;
		for (int i = 0; i < pBssList->dwNumberOfItems; i++) {
			pBssEntry = (WLAN_AVAILABLE_NETWORK*)& pBssList->Network[i];
			*_log << "Profile Name\t" << i << pBssEntry->strProfileName;

			if (pBssEntry->dot11Ssid.uSSIDLength == 0)
				wprintf(L"\n");
			else {
				for (int k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
					*_log << char((int)pBssEntry->dot11Ssid.ucSSID[k]);
					wprintf(L"%c", (int)pBssEntry->dot11Ssid.ucSSID[k]);
				}
				wprintf(L"\n");
			}
			*_log << "\n";
		}
	}
	*_log << "\n\n";
	return 0;
}



int WiFiConn::connect() {
	return 0;
}