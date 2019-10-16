#include "pch.h"
#include "WiFiConn.h"
#include "WlanApiErrorWrapper.h"
#include "StringHelper.h"

#include <iostream>
#include <algorithm>

WiFiConn::WiFiConn(){
	entries = new std::vector<WLAN_AVAILABLE_NETWORK>();
}

WiFiConn::~WiFiConn() {
	std::cout << "wifi auto connection end";

	delete entries;
}

bool WiFiConn::open_handle() {
	//Declare and initialize variables 
	DWORD dwMaxClient = 2;
	DWORD dwCurVersion = 2;
	
	//open handle
	auto result = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &wlan_handle);
	
	return WlanApiErrorWrapper::wrap_open_handle_result(result);
}

bool WiFiConn::get_interface_info() {
	//Declare and initialize variables.
	if (wlan_handle == nullptr) {
		auto status = open_handle();
		if (status == false)
			return false; // cannot open client handle
	}

	PWLAN_INTERFACE_INFO_LIST interface_list = nullptr;
	auto result = WlanEnumInterfaces(wlan_handle, NULL, &interface_list);

	const auto is_success = WlanApiErrorWrapper::wrap_enum_interface_result(result);
	
	if (!is_success)
		return false;

	//取出默认网卡
	pwlan_interface_info = interface_list->InterfaceInfo;
	return true;
	/**
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
	*/
}


bool WiFiConn::scan_wlan_list(int interface_index) {
	
	//Declare and initialize variables.
	PWLAN_AVAILABLE_NETWORK_LIST pnetwork_list;

	auto result = WlanGetAvailableNetworkList(wlan_handle, &(pwlan_interface_info->InterfaceGuid),
		WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES,
		NULL,
		&pnetwork_list);

	bool is_success = WlanApiErrorWrapper::wrap_get_network_list_result(result);
	
	if (!is_success)
		return false;

	//do available network list copy to private vector
	for (DWORD network_index = 0; network_index < pnetwork_list->dwNumberOfItems; network_index++)
		entries->push_back(pnetwork_list->Network[network_index]);

	WlanFreeMemory(pnetwork_list);
	return true;
	/**
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
*/
}


PWLAN_CONNECTION_PARAMETERS WiFiConn::build_wlan_parameters(WLAN_AVAILABLE_NETWORK entry, 
	const std::string& wifiname, const std::string& pass){
	//应该使用自动内存管理机制，不然在何处释放
	const auto params = new WLAN_CONNECTION_PARAMETERS();

	params->wlanConnectionMode = wlan_connection_mode_profile;

	const auto w_ssid_str = StringHelper::string_convert_wstring(wifiname);

	//转换为wstring的内存何时释放
	params->strProfile = static_cast<LPCWSTR>(w_ssid_str->c_str());
	params->pDot11Ssid = &entry.dot11Ssid;

	params->pDesiredBssidList = nullptr;
	params->dot11BssType = entry.dot11BssType;
	params->dwFlags = 0;

	return params;
}

bool WiFiConn::connect(std::string wifiname, std::string password) {
	//确定 wlan client 已经open
	if (wlan_handle == nullptr)
		if (!open_handle())
			return false;
	//网卡信息已经获取
	if (pwlan_interface_info == nullptr)
		if (!get_interface_info())
			return false;

	//real-time scanning network
	if (!scan_wlan_list())
		return false;

	const auto target_network = std::find_if(std::begin(*entries), std::end(*entries), [wifiname](WLAN_AVAILABLE_NETWORK entry) 
		{ return StringHelper::is_equal(entry.dot11Ssid.ucSSID, wifiname.c_str()); });

	if (target_network == std::end(*entries)){
		std::cout << "do not has this network: " << wifiname << std::endl;
		return false;
	}

	auto connect_parameters = build_wlan_parameters(*target_network, wifiname.c_str(), password.c_str());
	auto result  = 
	return 0;
}