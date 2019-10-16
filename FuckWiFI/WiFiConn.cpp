#include "pch.h"
#include "WiFiConn.h"

#include <iostream>

WiFiConn::WiFiConn(WiFiLog* log, const std::string wifiname) :_log(log), _wifiname(wifiname) {
	//_pIfList = NULL;
		//Declare and initialize variables.
	DWORD dwMaxClient = 2;   //    
	DWORD dwCurVersion = 0;
	DWORD dwResult = 0;

	//从服务器获取一个句柄, 假设会成功
	dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &_hClient);
	if (dwResult != ERROR_SUCCESS) {
		_log->logTime();
		*_log << "WlanOpenHandle failed with error: " << (int)dwResult << "\n";
		wprintf(L"WlanOpenHandle failed with error: %d\n", (int)dwResult);
		// FormatMessage can be used to find out why the function failed
	}
}

WiFiConn::~WiFiConn() {
	std::cout << "wifi auto connection end";
	if (_pIfList != NULL) {
		WlanFreeMemory(_pIfList);
		_pIfList = NULL;
	}
}

int WiFiConn::enumInterface() {
	//Declare and initialize variables.
	DWORD dwResult;
	int iRet = 0;
	WCHAR GuidString[40] = { 0 };
	int result_status = 1;

	dwResult = WlanEnumInterfaces(_hClient, NULL, &_pIfList);
	if (dwResult != ERROR_SUCCESS) {
		_log->logTime();
		wprintf(L"WlanEnumInterfaces failed with error: %d\n", (int)dwResult);
		*_log << "WlanEnumInterfaces failed with error: " << dwResult << "\n";
		// FormatMessage can be used to find out why the function failed
		result_status = 1;
	}
	else {
		_log->logTime();
		*_log << "WlanEnumInterfaces Success: " << dwResult << "\n";
		_log->plus_retract();
		wprintf(L"Num Entries: %d\n", _pIfList->dwNumberOfItems);
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
			wprintf(L"\tstatus:\t");
			switch (pIfInfo->isState) {
			case wlan_interface_state_not_ready:
				wprintf(L"Not ready\n");
				*_log << "Not ready\n";
				break;
			case wlan_interface_state_connected:
				*_log << "Connected\n";
				wprintf(L"connected\n");
				result_status = 0;
				break;
			case wlan_interface_state_ad_hoc_network_formed:
				wprintf(L"First node in a ad hoc network\n");
				*_log << "First node in a ad hoc network\n";
				break;
			case wlan_interface_state_disconnecting:
				wprintf(L"Disconnecting\n");
				*_log << "Disconnecting\n";
				break;
			case wlan_interface_state_disconnected:
				wprintf(L"Not connected\n");
				*_log << "Not connected\n";
				break;
			case wlan_interface_state_associating:
				wprintf(L"Attempting to associate with a network\n");
				*_log << "Attempting to associate with a network\n";
				break;
			case wlan_interface_state_discovering:
				wprintf(L"Auto configuration is discovering settings for the network\n");
				*_log << "Auto configuration is discovering settings for the network\n";
				break;
			case wlan_interface_state_authenticating:
				wprintf(L"In process of authenticating\n");
				*_log << "In process of authenticating\n";
				break;
			default:
				wprintf(L"Unknown state\n");
				*_log << "Unknown state %ld\n", pIfInfo->isState;
				break;
			}
			_log->retract_reset();
			*_log << "\n\n\n";
		}
	}
	return result_status;
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
		//wifi 名字buffer
		char tmpBuffer[256];
		memset(tmpBuffer, 0, 256);

		for (int i = 0; i < pBssList->dwNumberOfItems; i++) {
			pBssEntry = (WLAN_AVAILABLE_NETWORK*)& pBssList->Network[i];
			*_log << "Profile Name\t" << i <<"";

			if (pBssEntry->dot11Ssid.uSSIDLength == 0) {
				*_log << "\n";
			}
			else {
				for (int k = 0; k < pBssEntry->dot11Ssid.uSSIDLength; k++) {
					*_log << char((int)pBssEntry->dot11Ssid.ucSSID[k]);
				}
			}
			*_log << "\n";

			//与目标wifi名字进行比较
			if (is_target((char *)pBssEntry->dot11Ssid.ucSSID, pBssEntry->dot11Ssid.uSSIDLength)) {
				*_log << "<-Target WiFi->" << "\n";

				//WLAN_CONNECTION_PARAMETERS wlanConnPara;
				//wlanConnPara.wlanConnectionMode = wlan_connection_mode_discovery_unsecure;//wlan_connection_mode_discovery_unsecure;
				//wlanConnPara.strProfile = pBssEntry->strProfileName;

				//DOT11_SSID infoForSSID;
				////strcpy((char*)(infoForSSID.ucSSID), (char*)pBssEntry->dot11Ssid.ucSSID);
				////std::copy((char*)(infoForSSID.ucSSID), (char*)(infoForSSID.ucSSID)+ pBssEntry->dot11Ssid.uSSIDLength, (char*)pBssEntry->dot11Ssid.ucSSID);
				////strncpy((char*)(infoForSSID.ucSSID), (char*)pBssEntry->dot11Ssid.ucSSID, pBssEntry->dot11Ssid.uSSIDLength);
				//strcpy((char*)(infoForSSID.ucSSID), (char*)pBssEntry->dot11Ssid.ucSSID);
				//infoForSSID.uSSIDLength = pBssEntry->dot11Ssid.uSSIDLength;
				//
				//wlanConnPara.pDot11Ssid = &infoForSSID;
				//wlanConnPara.pDesiredBssidList = NULL;
				//wlanConnPara.dot11BssType = pBssEntry->dot11BssType;
				//wlanConnPara.dwFlags = pBssEntry->dwFlags;


				WLAN_CONNECTION_PARAMETERS connectionParams;
				connectionParams.wlanConnectionMode = wlan_connection_mode_profile;
				connectionParams.strProfile = NULL;
				std::string nameOfSSID = "swu-wifi";
				

				//string convert to wstring
				auto w_name = new std::wstring(std::begin(nameOfSSID), std::end(nameOfSSID));
				connectionParams.strProfile = static_cast<LPCWSTR>(w_name->c_str());

				int lengthOfSSID = nameOfSSID.size();
				DOT11_SSID infoForSSID;
				strncpy((char*)(infoForSSID.ucSSID), (char*)pBssEntry->dot11Ssid.ucSSID, pBssEntry->dot11Ssid.uSSIDLength);
				infoForSSID.uSSIDLength = lengthOfSSID;
				connectionParams.pDot11Ssid = &pBssEntry->dot11Ssid;
				connectionParams.pDesiredBssidList = NULL;
				connectionParams.dot11BssType = pBssEntry->dot11BssType;
				connectionParams.dwFlags = 0;

				wprintf(L"\tOpen\n");
				// Now connected with open networks
				//con = WlanConnect(hClient, &pInterface->InterfaceGuid, &connectionParams, NULL);s

				/* Connect to the host */
				auto status = WlanConnect(_hClient, &pIfInfo->InterfaceGuid, &connectionParams, NULL);
				if (status == ERROR_SUCCESS) {
					*_log << "connect success!!!";
					break;
				}
				else {
					*_log << "connect not success!!!";
					break;
				}

				//已经找到直接退出
				break;
			}
		}

		_log->flush();
	}
	*_log << "\n\n";
	return 0;
}

bool WiFiConn::is_target(const char* str, int length) {
	if (length != _wifiname.size())
		return false;
	for (int i = 0; i < _wifiname.size(); i++)
		if (str[i] != str[i])
			return false;
	return true;
}

PWLAN_CONNECTION_PARAMETERS WiFiConn::build_wlan_parameters(WLAN_AVAILABLE_NETWORK entry, const char* ssid,
	const char* pass)
{
	const auto params = new WLAN_CONNECTION_PARAMETERS();

	params->wlanConnectionMode = wlan_connection_mode_profile;

	const std::string ssid_str(ssid);
	//const auto w_ssid_str = StringHelper::convert_string_to_w_string(ssid_str);

	//params->strProfile = static_cast<LPCWSTR>(w_ssid_str->c_str());
	params->pDot11Ssid = &entry.dot11Ssid;

	params->pDesiredBssidList = nullptr;
	params->dot11BssType = entry.dot11BssType;
	params->dwFlags = 0;

	return params;
}

int WiFiConn::connect() {

	_log->flush();
	return 0;
}