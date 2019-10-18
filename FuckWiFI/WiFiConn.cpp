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
	if (pwlan_interface_list != nullptr)
		WlanFreeMemory(pwlan_interface_list);

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

	const bool is_success = WlanApiErrorWrapper::wrap_enum_interface_result(result);
	
	if (!is_success)
		return false;

	//释放以前存储网卡信息的内存
	//这个使库里边的 ptr, 不需要自己手动释放
	//if (pwlan_interface_info != nullptr)
	//	WlanFreeMemory(pwlan_interface_info);
	
	//没有网卡，返回false
	if (interface_list->dwNumberOfItems == 0) {
		WlanFreeMemory(interface_list);
		return false;
	}

	//取出默认网卡
	WlanFreeMemory(pwlan_interface_list); //释放前期获取的内存
	pwlan_interface_list = interface_list;
	return true;
}

int WiFiConn::get_interface_status() {
	std::string temp;  // no use
	return get_interface_status(temp);
}

int WiFiConn::get_interface_status(std::string& desc ) {
	int result = 1;

	auto interface_result = get_interface_info();
	if (!interface_result) {
		desc = "not open hard wifi adapter";
		std::cout <<  "not open hard wifi adapter" << std::endl;
		return -1;
	}

	static int cnt = 0;
	std::cout << "get interface status times: " << cnt++ << std::endl;

	switch (pwlan_interface_list->InterfaceInfo->isState) {
	case wlan_interface_state_not_ready:
		desc = "Not ready"; result = -2; break; //没有准备好
	
	case wlan_interface_state_connected:
		desc = "Connected"; result = 1; break;
	
	case wlan_interface_state_ad_hoc_network_formed:
		desc = "First node in a ad hoc network"; break;
	
	case wlan_interface_state_disconnecting:
		desc = "Disconnecting";  break;
	
	case wlan_interface_state_disconnected:
		desc = "Not connected"; result = 0; break;
	
	case wlan_interface_state_associating:
		desc = "Attempting to associate with a network"; break;

	case wlan_interface_state_discovering:
		desc = "Auto configuration is discovering settings for the network"; break;

	case wlan_interface_state_authenticating:
		desc = "In process of authenticating"; break;
	default:
		desc = "Unknown state"; break;
	}

	std::cout << "adapter status: " << desc << std::endl;

	return result;
}

bool WiFiConn::scan_wlan_list(int interface_index) {
	
	//Declare and initialize variables.
	PWLAN_AVAILABLE_NETWORK_LIST pnetwork_list;

	if (wlan_handle == nullptr)
		if (!open_handle())
			return false;

	if ( pwlan_interface_list == nullptr)
		if (!get_interface_info())
			return false;

	auto result = WlanGetAvailableNetworkList(wlan_handle, 
		&(pwlan_interface_list->InterfaceInfo->InterfaceGuid),
		WLAN_AVAILABLE_NETWORK_INCLUDE_ALL_ADHOC_PROFILES,
		NULL,
		&pnetwork_list);

	bool is_success = WlanApiErrorWrapper::wrap_get_network_list_result(result);
	
	if (!is_success)
		return false;

	entries->clear();
	//do available network list copy to private vector
	for (DWORD network_index = 0; network_index < pnetwork_list->dwNumberOfItems; network_index++)
		entries->push_back(pnetwork_list->Network[network_index]);

	WlanFreeMemory(pnetwork_list);
	return true;
	
}


std::shared_ptr<WLAN_CONNECTION_PARAMETERS>  WiFiConn::build_wlan_parameters(WLAN_AVAILABLE_NETWORK entry,
	const std::wstring* pwifiname, const std::string& pass){
	//使用自动内存管理机制，不然在何处释放

	auto pparams = std::make_shared<WLAN_CONNECTION_PARAMETERS>();

	pparams->wlanConnectionMode = wlan_connection_mode_profile;



	//转换为wstring的内存何时释放
	pparams->strProfile = static_cast<LPCWSTR>(pwifiname->c_str());
	pparams->pDot11Ssid = &entry.dot11Ssid;

	pparams->pDesiredBssidList = nullptr;
	pparams->dot11BssType = entry.dot11BssType;
	pparams->dwFlags = 0;

	return pparams;
}

bool WiFiConn::connect(std::string wifiname, std::string password) {
	//确定 wlan client 已经open
	if (wlan_handle == nullptr)
		if (!open_handle())
			return false;
	//网卡信息已经获取
	if (pwlan_interface_list == nullptr)
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

	const auto w_ssid_str = StringHelper::string_convert_wstring(wifiname);
	auto connect_parameters = build_wlan_parameters(*target_network, w_ssid_str.get(), password.c_str());
	
	static size_t times = 0;
	std::cout << "wlan connect times: " << times << std::endl;

	auto result = WlanConnect(wlan_handle, 
		&pwlan_interface_list->InterfaceInfo->InterfaceGuid, 
		connect_parameters.get(), 
		nullptr);
	


	//回环函数怎么写
	std::cout << "connect " << wifiname << " success!!" << std::endl;
	std::cout << entries->size() << "-------" << entries->capacity() << std::endl;
	return  WlanApiErrorWrapper::wrap_connect_result(result);
}