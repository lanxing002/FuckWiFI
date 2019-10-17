// FuckWiFI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include "WiFiLog.h"
#include "WiFiConn.h"
#include <iostream>

#include <vld.h>

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

void connetcWifi() {
	//EnableMemLeakCheck();
	std::string log_path("ConnectWifi.log");
	WiFiLog wifilog(log_path);

	std::string wifiname("swu-wifi");
	WiFiConn awifiConn;
	unsigned int sleepTime =  1 * 100;

	int cnt = 0;
	while (true) {
		if (awifiConn.get_interface_status() == 0) {
			continue;  //there is no interface

		}
		awifiConn.connect(wifiname, "");
		//Sleep(sleepTime);
	}
	
	return;
}

int main()
{
	connetcWifi();
	return 0;
}
