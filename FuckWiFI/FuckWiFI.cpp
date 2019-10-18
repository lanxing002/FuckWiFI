// FuckWiFI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include "WiFiLog.h"
#include "WiFiConn.h"
#include <iostream>

//#include <vld.h>

// Need to link with Wlanapi.lib and Ole32.lib
#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

void connetcWifi() {

	std::string wifiname("swu-wifi");
	unsigned int sleepTime =  60 * 1000;

	int cnt = 0;
	while (true) {
		Sleep(sleepTime);
		//已经连接状态
		{
			WiFiConn awifiConn;
			auto interface_status = awifiConn.get_interface_status();
			if (interface_status != 0) {
				std::cout << "statucs ---> :" << interface_status << std::endl;
				continue;  //there is no interface or apapter not ready to reconnect
			}
			awifiConn.connect(wifiname, "");
		}
	}
	
	return;
}

int main()
{
	connetcWifi();
	return 0;
}
