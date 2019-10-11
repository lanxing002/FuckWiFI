#include "pch.h"
#include "WiFiConn.h"

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

WiFiConn::~WiFiConn() {
	std::cout << "wifi auto connection end";
	//if (_pIfList != NULL) {
	//	WlanFreeMemory(_pIfList);
	//	_pIfList = NULL;
	//}
}

bool WiFiConn::connect(std::string wifiName, int interface_index) {
	return true;
}

int WiFiConn::enumInterface() {

	//Declare and initialize variables.
	DWORD dwMaxClient = 2;   //    
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	int iRet = 0;

	WCHAR GuidString[40] = { 0 };

	/* variables used for WlanEnumInterfaces  */

	
	PWLAN_INTERFACE_INFO pIfInfo = NULL;

	//从服务器获取一个句柄
	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &_hClient);
	if (dwResult != ERROR_SUCCESS) {
		_log->logTime();
		*_log << "WlanOpenHandle failed with error: " << (int)dwResult << "\n";
		// FormatMessage can be used to find out why the function failed
		return 1;
	}

	PWLAN_INTERFACE_INFO_LIST _pIfList;
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
		*_log << "	Num Entries: " << _pIfList->dwNumberOfItems << "\n";
		*_log << "	Current Index: " << _pIfList->dwIndex << "\n";

		////默认使用第一张网卡；
		//pIfInfo = (WLAN_INTERFACE_INFO*)& pIfList->InterfaceInfo[0];
		//scanWlanList(hClient, pIfInfo->InterfaceGuid);

		for (int i = 0; i < (int)_pIfList->dwNumberOfItems; i++) {
			pIfInfo = (WLAN_INTERFACE_INFO *)&_pIfList->InterfaceInfo[i];
			*_log << "		Interface Index[" << i << "]\n";
			iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString, 39);
			// For c rather than C++ source code, the above line needs to be
			// iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39);
			if (iRet == 0)
				*_log << "		StringFromGUID2 failed\n";
			else {
				*_log << "		InterfaceGUID: " << GuidString << "\n";
			}

			*_log << "		Interface Description[" << i << "]:" << pIfInfo->strInterfaceDescription << "\n";
			*_log << "		Interface State[" << i << "]:\t";
			switch (pIfInfo->isState) {
			case wlan_interface_state_not_ready:
				*_log << "		Not ready\n";
				break;
			case wlan_interface_state_connected:
				*_log << "		Connected\n";
				break;
			case wlan_interface_state_ad_hoc_network_formed:
				*_log << "		First node in a ad hoc network\n";
				break;
			case wlan_interface_state_disconnecting:
				*_log << "		Disconnecting\n";
				break;
			case wlan_interface_state_disconnected:
				*_log << "		Not connected\n";
				break;
			case wlan_interface_state_associating:
				*_log << "		Attempting to associate with a network\n";
				break;
			case wlan_interface_state_discovering:
				*_log << "		Auto configuration is discovering settings for the network\n";
				break;
			case wlan_interface_state_authenticating:
				*_log << "		In process of authenticating\n";
				break;
			default:
				*_log << "		Unknown state %ld\n", pIfInfo->isState;
				break;
			}
			*_log << "\n";
		}
	}
	return 0;
}
/**
int WiFiConn::getAvaiableNet() {
	static const WCHAR targetNetWork[] = { 's', 'w', 'u', '-', 'w', 'i', 'f', 'i', '\0' };
	//Declare and initialize variables.
	//HANDLE hClient = NULL;
	DWORD dwResult = 0;
	int iRet = 0;


	PDOT11_SSID interfaceSSID = NULL;
	PWLAN_RAW_DATA ramData = NULL;
	PVOID pvoid;
	PWLAN_AVAILABLE_NETWORK_LIST pBssList;

	//dwResult = WlanScan(hClient, &interfaceGUID, interfaceSSID, ramData, NULL);
	//if (dwResult != ERROR_SUCCESS) {
	//	wprintf(L"Wlan Scan Network failed with error: %u\n", dwResult);
	//}
			
	//默认使用第一张网卡；
	PWLAN_INTERFACE_INFO pIfInfo = NULL;
	pIfInfo = (WLAN_INTERFACE_INFO*)& _pIfList->InterfaceInfo[0];

	dwResult = WlanGetAvailableNetworkList(_hClient, &(pIfInfo->InterfaceGuid),
		WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES,
		NULL,
		&pBssList);

	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"Wlan Scan Network failed with error: %u\n", dwResult);
		return 1;
	}
	else {
		wprintf(L"  Num Entries: %lu\n\n", pBssList->dwNumberOfItems);
		PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;
		for (int i = 0; i < pBssList->dwNumberOfItems; i++) {
			pBssEntry = (WLAN_AVAILABLE_NETWORK*)& pBssList->Network[i];
			wprintf(L"  Profile Name[%u]:  %ws\n", i, pBssEntry->strProfileName);
			if (wcscmp(pBssEntry->strProfileName, targetNetWork) == 0) {
				wprintf(L"found______________________.....", i, pBssEntry->strProfileName);
				//connect the wifi
			}

			if (pBssEntry->dot11Ssid.uSSIDLength == 0)
				wprintf(L"\n");
			else {
				for (int k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
					wprintf(L"%c", (int)pBssEntry->dot11Ssid.ucSSID[k]);
				}
				wprintf(L"\n");
			}
		}
	}
	return 0;
}
*/


int WiFiConn::connect() {
	return 0;
}