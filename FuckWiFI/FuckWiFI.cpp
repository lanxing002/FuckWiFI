// FuckWiFI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>



#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#include <stdio.h>
#include <stdlib.h>

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

/**
	interface guid 是网卡的唯一描述符

*/
int scanWlanList(HANDLE hClient, GUID interfaceGUID) {


	//Declare and initialize variables.
	//HANDLE hClient = NULL;
	DWORD dwResult = 0;
	int iRet = 0;

	/**
	DWORD WlanScan(
		HANDLE               hClientHandle,  
		const GUID * pInterfaceGuid,  //网卡唯一描述符
		const PDOT11_SSID    pDot11Ssid, //ssid, 设置为null
		const PWLAN_RAW_DATA pIeData, //扫描网络的必要信息，比如820.1协议什么的
		PVOID                pReserved //保留未来使用
	);
	*/

	PDOT11_SSID interfaceSSID = NULL;
	PWLAN_RAW_DATA ramData = NULL;
	PVOID pvoid;
	PWLAN_AVAILABLE_NETWORK_LIST pBssList;

	//dwResult = WlanScan(hClient, &interfaceGUID, interfaceSSID, ramData, NULL);
	//if (dwResult != ERROR_SUCCESS) {
	//	wprintf(L"Wlan Scan Network failed with error: %u\n", dwResult);
	//}

	dwResult = WlanGetAvailableNetworkList(hClient, &interfaceGUID, 
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

int wmain()
{

	//Declare and initialize variables.

	HANDLE hClient = NULL;
	DWORD dwMaxClient = 2;   //    
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;
	int iRet = 0;

	WCHAR GuidString[40] = { 0 };

	int i;

	/* variables used for WlanEnumInterfaces  */

	PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;


	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanOpenHandle failed with error: %u\n", dwResult);
		// FormatMessage can be used to find out why the function failed
		return 1;
	}

	dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	if (dwResult != ERROR_SUCCESS) {
		wprintf(L"WlanEnumInterfaces failed with error: %u\n", dwResult);
		// FormatMessage can be used to find out why the function failed
		return 1;
	}
	else {
		wprintf(L"Num Entries: %lu\n", pIfList->dwNumberOfItems);
		wprintf(L"Current Index: %lu\n", pIfList->dwIndex);

		//默认使用第一张网卡；
		pIfInfo = (WLAN_INTERFACE_INFO*)& pIfList->InterfaceInfo[0];
		scanWlanList(hClient, pIfInfo->InterfaceGuid);
		/**
		for (i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
			pIfInfo = (WLAN_INTERFACE_INFO *)&pIfList->InterfaceInfo[i];
			wprintf(L"  Interface Index[%d]:\t %lu\n", i, i);
			iRet = StringFromGUID2(pIfInfo->InterfaceGuid, (LPOLESTR)&GuidString, 39);
			// For c rather than C++ source code, the above line needs to be
			// iRet = StringFromGUID2(&pIfInfo->InterfaceGuid, (LPOLESTR) &GuidString, 39); 
			if (iRet == 0)
				wprintf(L"StringFromGUID2 failed\n");
			else {
				wprintf(L"  InterfaceGUID[%d]: %ws\n", i, GuidString);
			}
			wprintf(L"  Interface Description[%d]: %ws", i,
				pIfInfo->strInterfaceDescription);
			wprintf(L"\n");
			wprintf(L"  Interface State[%d]:\t ", i);
			switch (pIfInfo->isState) {
			case wlan_interface_state_not_ready:
				wprintf(L"Not ready\n");
				break;
			case wlan_interface_state_connected:
				wprintf(L"Connected\n");
				break;
			case wlan_interface_state_ad_hoc_network_formed:
				wprintf(L"First node in a ad hoc network\n");
				break;
			case wlan_interface_state_disconnecting:
				wprintf(L"Disconnecting\n");
				break;
			case wlan_interface_state_disconnected:
				wprintf(L"Not connected\n");
				break;
			case wlan_interface_state_associating:
				wprintf(L"Attempting to associate with a network\n");
				break;
			case wlan_interface_state_discovering:
				wprintf(L"Auto configuration is discovering settings for the network\n");
				break;
			case wlan_interface_state_authenticating:
				wprintf(L"In process of authenticating\n");
				break;
			default:
				wprintf(L"Unknown state %ld\n", pIfInfo->isState);
				break;
			}
			wprintf(L"\n");
		}
		*/
	}

	if (pIfList != NULL) {
		WlanFreeMemory(pIfList);
		pIfList = NULL;
	}
	return 0;
}

int main()
{
	wmain();
    std::cout << "Hello World!\n"; 
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
