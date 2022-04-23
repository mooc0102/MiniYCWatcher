#pragma once
#include <iostream>
#include <windows.h>
#include <fltuser.h>
#include "minispy.h"
#include "filesystemevent.h"
#include <vector>
#include "FuncTool.h"
#include <functional>
#include <future>
#define PORT_NAME L"\\MiniYCWatcherPort"
#define BUFFER_SIZE 4096

class FilterMgr
{
public:
	FilterMgr();
	void connect();

	bool connected();

	HRESULT SendAndRead(
		_In_ COMMAND_MESSAGE commandMessage,
		_Out_ PUCHAR buffer,
		_Out_ DWORD &bytesReturned);

	HRESULT Send(COMMAND_MESSAGE* commandMessage, BYTE data[], int dataSize);


	bool checkConnectStatus();
	
	
private:
	

	HANDLE port;
	HRESULT isConnected;
	

	

};