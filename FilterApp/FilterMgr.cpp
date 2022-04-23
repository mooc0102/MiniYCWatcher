#include "FilterMgr.h"


	FilterMgr::FilterMgr()
	{
		
		port = INVALID_HANDLE_VALUE;
		isConnected = S_OK;
	}

	void FilterMgr::connect() 
	{
		std::cout << "filterMgr call communication port" << std::endl;
		isConnected = FilterConnectCommunicationPort(PORT_NAME, 0, NULL, 0, NULL, &port);
		connected();
	}
	 
	bool FilterMgr::connected()
	{
		if (IS_ERROR(isConnected)) {
			std::cout 
				<< "Could not connect to filter : " 
				<< isConnected 
				<< std::endl;
			return false;
		}
		return true;
	}

	HRESULT FilterMgr::Send(COMMAND_MESSAGE* commandMessage, BYTE data[], int dataSize)
	{

		int size = sizeof(commandMessage) + dataSize;
		DWORD resultSize = 0;
		
		memcpy_s(commandMessage->Data, dataSize ,data, dataSize);

		HRESULT sendMsgResult = FilterSendMessage(
			port,
			commandMessage,
			size,
			NULL,
			0,
			&resultSize);
		//std::cout << sendMsgResult << std::endl;
		return sendMsgResult;
	}


	HRESULT FilterMgr::SendAndRead(
		_In_ COMMAND_MESSAGE commandMessage,
		_Out_ PUCHAR buffer,
		_Out_ DWORD &bytesReturned)
	{
		HRESULT sendMsgResult;

		if (checkConnectStatus()) 
		{

			sendMsgResult = FilterSendMessage(
				port,
				&commandMessage,
				sizeof(commandMessage),
				buffer,
				BUFFER_SIZE,
				&bytesReturned);


			return sendMsgResult;
		}
		else 
		{
			std::cout
				<< "filter has not been connected."
				<< isConnected
				<< std::endl;
			return S_FALSE;
		}
	}
	
	//true for connected
	bool FilterMgr::checkConnectStatus() 
	{
		return SUCCEEDED(isConnected);
	}

	

	
	
