#include "EventWatcher.h"

EventWatcher::EventWatcher(std::string Path)
{
	fltmgr = FilterMgr();
	path = Path;
}
void EventWatcher::connectAndWatch() 
{

	fltmgr.connect();

	NotWatchProcess(GetCurrentProcessId());

	

	if (SUCCEEDED(SetPath(path))) 
	{
		std::cout << "set path successed:" << path << std::endl;
	}
	else 
	{
		std::cout << "set path failed" << std::endl;
	}
	 
	
	futureObj = exitSignal.get_future();
	std::thread th(&EventWatcher::ForwardEvents, this, std::move(futureObj));

	std::string commandKey ;
	int commandValue = 0;
	Sleep(500);
	std::cout << "Input Example:" << std::endl;
	std::cout << "wait 0 --means pause now" << std::endl;
	std::cout << "wait x --means wait x seconds and pause, if x < 0 means terminal." << std::endl;
	std::cout << "continue --means resume" << std::endl;
	
	while (std::cin >> commandKey)
	{
		if (commandKey.compare("wait") == 0) 
		{
			std::cin >> commandValue;
			if (commandValue >= 0) 
			{
				Sleep(commandValue);
				EventWatcher::StopRequest();
				th.join();
			}
			else 
			{
				EventWatcher::StopRequest();
				th.join();
				break;
			
			}
		}
		else if (commandKey.compare("continue") == 0) 
		{
			exitSignal = std::promise<void>();
			futureObj = std::future<void>();
			futureObj = exitSignal.get_future();
			th = std::thread(&EventWatcher::ForwardEvents, this, std::move(futureObj));
		}
	}

	
	
}

void EventWatcher::NotWatchProcess(long long processId)
{
	WatchProcess(-1 * processId);

}

void EventWatcher::WatchProcess(long long  processId)
{
	COMMAND_MESSAGE* message = (COMMAND_MESSAGE*)malloc(sizeof(COMMAND_MESSAGE) + 8);
	message->Command = MINIYCWATCHER_COMMAND::SetWatchProcess;
	BYTE a[8] = {0,0,0,0,0,0,0,0};
	printf("process id: %lld\n", processId);
	longlongToBytes(processId, a);
	HRESULT setProcessResult = fltmgr.Send(message, a, 8);

}
HRESULT EventWatcher::SetPath(
	_In_ std::string path) 
{

	std::wstring wtemp(path.begin(), path.end());
	int i = wtemp.length();
	int siz = sizeof(wchar_t) * (i + 1);

	BYTE* b = (BYTE*)malloc(siz);
	memcpy(b, wtemp.c_str(), siz);
	wchar_t* q = (wchar_t*)b;


	COMMAND_MESSAGE* message = (COMMAND_MESSAGE*)malloc(sizeof(COMMAND_MESSAGE) + siz);
	DWORD bytesReturned = 0;
	message->Command = MINIYCWATCHER_COMMAND::SetPathFilter;
	message->Reserved = 0;

	return fltmgr.Send(message, b, siz);
}
void EventWatcher::longlongToBytes(long long num, BYTE arr[8])
{
	arr[7] = ((num >> 56) & 0xFF);
	arr[6] = ((num >> 48) & 0xFF);
	arr[5] = ((num >> 40) & 0xFF);
	arr[4] = ((num >> 32) & 0xFF);
	arr[3] = ((num >> 24) & 0xFF);
	arr[2] = ((num >> 16) & 0xFF);
	arr[1] = ((num >> 8) & 0xFF);
	arr[0] = (num & 0xFF);
}

void EventWatcher::StopRequest()
{
	std::cout << "stop event happened" << std::endl;
	exitSignal.set_value();
}

std::vector<FileSystemEvent> EventWatcher::GetFileSystemEvents(COMMAND_MESSAGE message, PUCHAR buffer)
{
	DWORD resultSize = 0;

	HRESULT hResult = fltmgr.SendAndRead(message, buffer, resultSize);

	if (IS_ERROR(hResult))
	{
		if (HRESULT_CODE(hResult) != ERROR_NO_MORE_ITEMS)
		{
			std::cout << "error happened" << std::endl;
		}

		return std::vector<FileSystemEvent>();
	}
	else 
	{
		//std::cout << "event get" << std::endl;
		return ReadFromBuffer(buffer, resultSize);
	}
}

std::vector<FileSystemEvent> EventWatcher::ReadFromBuffer(
	PUCHAR buffer,
	long bufferSize)
{
	std::vector<FileSystemEvent> events = std::vector<FileSystemEvent>();
	int offset = 0;
	while (offset + sizeof(LOG_RECORD) < bufferSize)
	{

		PUCHAR recordAddress = buffer + offset;
		PLOG_RECORD precord = (PLOG_RECORD)recordAddress;
		int stringBytes = precord->Length - sizeof(LOG_RECORD);
		std::vector<std::wstring> strings;

		

		if (precord->Data.EventType == UEventType::Move)
		{
			strings = ReadEventStringsFromBuffer(recordAddress, stringBytes);
			FileSystemEvent roe;
			roe.Filename = strings[1];
			roe.OldFilename = strings[0];
			roe.ProcessId = precord->Data.ProcessId;
			roe.Type = precord->Data.EventType;
			events.push_back(roe);
		}
		else
		{
			FileSystemEvent fse;
			switch (precord->Data.EventType) 
			{
			case UEventType::Change:
			case UEventType::Create:
			case UEventType::Delete:
			case UEventType::RealDelete:
			case UEventType::Overwrite:
	
				strings.push_back(precord->Names);
				fse.Filename = strings[0];
				fse.ProcessId = precord->Data.ProcessId;
				fse.Type = precord->Data.EventType;
				events.push_back(fse);
				break;
			case UEventType::Unknown1:
				break;
			default:
				break;
			}
		}
		offset += precord->Length;
	}
	return events;
}
std::vector<std::wstring> EventWatcher::ReadEventStringsFromBuffer(PUCHAR recordAddress, int stringBytes)
{
	PWCHAR stringOffset = (PWCHAR)(recordAddress + sizeof(LOG_RECORD));




	return FuncTool::charArray2VecStr(stringOffset, '\0', stringBytes);

}
//
//void EventWatcher::ForwardEvents()
//{
//	
//	while (1)
//	{
//		COMMAND_MESSAGE message;
//		message.Command = MINIYCWATCHER_COMMAND::GetMiniSpyLog;
//		UCHAR buffer[4096];
//		std::vector<FileSystemEvent> result = GetFileSystemEvents(message, buffer);
//		std::for_each(result.begin(), result.end(), [&](FileSystemEvent f) {DeliverEvent(f); });
//		std::this_thread::sleep_for(std::chrono::milliseconds(100));
//	}
//}








void EventWatcher::ForwardEvents(std::future<void> futureObj)
{
	std::cout << "Thread Start" << std::endl;
	while (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
	{
		COMMAND_MESSAGE message;
		message.Command = MINIYCWATCHER_COMMAND::GetMiniSpyLog;
	    UCHAR buffer[4096];
		std::vector<FileSystemEvent> result = GetFileSystemEvents(message, buffer);
		std::for_each(result.begin(), result.end(), [&](FileSystemEvent f) {DeliverEvent(f); });


		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	std::cout << "Thread End" << std::endl;
}


void EventWatcher::DeliverEvent(FileSystemEvent fileEvent)
{
	switch (fileEvent.Type)
	{
	case UEventType::Change:
		handleFSEvent(fileEvent.Filename, fileEvent.ProcessId, onChanged);
		break;
	case UEventType::Create:
		handleFSEvent(fileEvent.Filename, fileEvent.ProcessId, onCreate);
		break;
	case UEventType::Delete:
		handleFSEvent(fileEvent.Filename, fileEvent.ProcessId, onDelete);
		break;
	case UEventType::Move:
		handleRNMEvent(fileEvent.Filename, fileEvent.OldFilename, fileEvent.ProcessId, onMoved);
		break;
	case UEventType::RealDelete:
		handleFSEvent(fileEvent.Filename, fileEvent.ProcessId, onRealDelete);
		break;
	case UEventType::Overwrite:
		handleFSEvent(fileEvent.Filename, fileEvent.ProcessId, onOverwrited);
		break;
	default:
		break;
	}
}


void EventWatcher::handleFSEvent(std::wstring name, unsigned long long processId, fsFunPtr f)
{
	f(name, processId);
}

void EventWatcher::handleRNMEvent(std::wstring name, std::wstring oldName, unsigned long long processId, rnmFunPtr r)
{
	r(name, oldName, processId);
}


void EventWatcher::onCreate(std::wstring name, unsigned long long processId)
{
	//std::wcout << "Created: " << name << std::endl;
	printf("Created: %ls\n", name.c_str());


}

void EventWatcher::onChanged(std::wstring name, unsigned long long processId)
{
	//std::wcout << "Changed: " << name << std::endl;
	printf("Changed: %ls\n", name.c_str());
}

void EventWatcher::onDelete(std::wstring name, unsigned long long processId)
{
	//std::wcout << "Deleted: " << name << std::endl;
	printf("Deleted: %ls\n", name.c_str());
}

void EventWatcher::onRealDelete(std::wstring name, unsigned long long processId)
{
	//std::wcout << "Deleted: " << name << std::endl;
	printf("Real Deleted: %ls\n", name.c_str());
}

void EventWatcher::onMoved(std::wstring name, std::wstring oldName, unsigned long long processId)
{
	printf("Moved: %ls -> %ls\n",oldName.c_str(), name.c_str());
	//std::wcout << "Moved: " << oldName <<" -> " << name <<std::endl;
}
void EventWatcher::onOverwrited(std::wstring name, unsigned long long processId)
{
	//std::wcout << "Changed: " << name << std::endl;
	printf("Overwrite: %ls\n", name.c_str());
}


