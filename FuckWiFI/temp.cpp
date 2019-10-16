#include "pch.h"
#ifndef UNICODE
#define UNICODE
#endif

#pragma comment(lib, "wlanapi.lib")

#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <string.h>

#include <wlanapi.h>

int wmain()
{
	DWORD dwResult;
	DWORD con;
	bool status = 0;
	HANDLE hClient;
	DWORD dwRealAPIVer;

	PWLAN_INTERFACE_INFO_LIST pInterfaces;
	PWLAN_INTERFACE_INFO pInterface;

	PWLAN_AVAILABLE_NETWORK_LIST pNetworks;
	PWLAN_AVAILABLE_NETWORK pNetwork;

	// 1) Connecting Wi-Fi
	dwResult = WlanOpenHandle(2, NULL, &dwRealAPIVer, &hClient);
	if (FAILED(dwResult))
	{
		wprintf(L"Can't connect Wi-Fi! Problem function: WlanOpenHandle.");
		_getch();
		return 1;
	}

	// 2) Enumerating Wi-Fi adapters (interfaces)
	dwResult = WlanEnumInterfaces(hClient, NULL, &pInterfaces);
	if (FAILED(dwResult))
	{
		wprintf(L"Can't enumerate WLAN adapters! Problem function: WlanEnumInterfaces.");
		_getch();
		return 2;
	}
	if (pInterfaces->dwNumberOfItems == 0)
	{
		wprintf(L"No Wi-Fi adapter found.");
		_getch();
		return 3;
	}

	// 3) Getting current Wi-Fi adapter.
	pInterface = &pInterfaces->InterfaceInfo[pInterfaces->dwIndex];

	// 4) Getting available networks for this adapter.
	dwResult = WlanGetAvailableNetworkList(hClient, &pInterface->InterfaceGuid, 0, NULL, &pNetworks);
	if (FAILED(dwResult))
	{
		wprintf(L"Can't get available networks! Probably your PC in Airplane mode, and/or Wifi turned out. Problem function: WlanGetAvailableNetworkList.");
		_getch();
		return 4;
	}

	// 5) Enumertating networks
	for (int i = 0; i < (int)pNetworks->dwNumberOfItems; i++)
	{
		// 6) Getting current network from list
		pNetwork = &pNetworks->Network[i];

		// 7) Getting SSID (network's name)
		DOT11_SSID oName = pNetwork->dot11Ssid;
		DOT11_AUTH_ALGORITHM auth = pNetwork->dot11DefaultAuthAlgorithm;
		if (oName.uSSIDLength == 0)
		{
			wprintf(L"(Unknown network name)");
		}
		else
		{
			// 8) Output all characters of SSID
			for (int j = 0; j < (int)oName.uSSIDLength; j++)
			{
				wprintf(L"%c", oName.ucSSID[j]);
			}
		}
		if (auth == 1)
		{
			// parameter for wlanconnect
			WLAN_CONNECTION_PARAMETERS connectionParams;
			connectionParams.wlanConnectionMode = wlan_connection_mode_discovery_unsecure;
			connectionParams.strProfile = NULL;
			const char* nameOfSSID = "swu-wifi";
			int lengthOfSSID = strlen((const char*)nameOfSSID);
			DOT11_SSID infoForSSID;
			strncpy((char*)(infoForSSID.ucSSID), nameOfSSID, lengthOfSSID);
			infoForSSID.uSSIDLength = lengthOfSSID;
			connectionParams.pDot11Ssid = &infoForSSID;
			connectionParams.pDesiredBssidList = NULL;
			connectionParams.dot11BssType = dot11_BSS_type_independent;
			connectionParams.dwFlags = WLAN_CONNECTION_ADHOC_JOIN_ONLY;

			wprintf(L"\tOpen ");
			// Now connected with open networks
			con = WlanConnect(hClient, &pInterface->InterfaceGuid, &connectionParams, NULL);
			if (FAILED(con))
			{
				wprintf(L"connection not working");
				_getch();
				return 5;
			}
			else
			{
				wprintf(L"connected succesfully");
			}
		}
		else
		{
			wprintf(L"\tSecure");
		}
		wprintf(L"\n");
	}

	_getch();

	// 9) Removing unnecessary lists from RAM
	if (pInterfaces != NULL)
	{
		WlanFreeMemory(pInterfaces);
	}
	if (pNetworks != NULL)
	{
		WlanFreeMemory(pNetworks);
	}

	return 0;
}