// FuckWiFI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "WiFiLog.h"
#include "WiFiConn.h"
#include <iostream>



// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

void connetcWifi() {
	std::string log_path("ConnectWifi.log");
	WiFiLog wifilog(log_path);

	std::string wifiname("swu-wifi");
	WiFiConn awifiConn(&wifilog, wifiname);
	unsigned int sleepTime = 6 * 1000;

	while (true) {
		Sleep(sleepTime);
		auto status = awifiConn.enumInterface();
		if (status == 0) 
			continue;  //connected state TODO:confirm connected wifi name
		awifiConn.getAvaiableNet();
	}
	return;
}

int main()
{
	connetcWifi();
	return 0;
}
