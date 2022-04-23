#pragma once
#include <iostream>
#include <windows.h>
#include <fltuser.h>
#include "minispy.h"
#include "filesystemevent.h"
#include <vector>
#include "FuncTool.h"
#include <thread>
#include <future>
#include "FilterMgr.h"
class EventWatcher
{

public:
	EventWatcher(std::string Path);
	typedef void (*fsFunPtr)(std::wstring name, unsigned long long processId);
	typedef void (*rnmFunPtr)(std::wstring name, std::wstring oldName, unsigned long long processId);
	void StopRequest();
	void connectAndWatch();
	void NotWatchProcess(long long processId);
	void WatchProcess(long long processId);
	void longlongToBytes(long long num, BYTE arr[8]);
	HRESULT SetPath(_In_ std::string path);

private:
	FilterMgr fltmgr;
	std::string path;
	std::promise<void> exitSignal;
	std::future<void> futureObj;

	std::vector<FileSystemEvent> GetFileSystemEvents(
		COMMAND_MESSAGE message,
		PUCHAR buffer);

	std::vector<FileSystemEvent> ReadFromBuffer(
		PUCHAR buffer,
		long bufferSize);
	std::vector<std::wstring> ReadEventStringsFromBuffer(
		PUCHAR recordAddress,
		int stringBytes);

	void handleFSEvent(std::wstring name, unsigned long long processId, fsFunPtr f);

	void handleRNMEvent(std::wstring name, std::wstring oldName, unsigned long long processId, rnmFunPtr r);


	void DeliverEvent(FileSystemEvent fileEvent);

	static void onCreate(std::wstring name, unsigned long long processId);
	static void onChanged(std::wstring name, unsigned long long processId);
	static void onOverwrited(std::wstring name, unsigned long long processId);
	static void onDelete(std::wstring name, unsigned long long processId);
	static void onRealDelete(std::wstring name, unsigned long long processId);
	static void onMoved(std::wstring name, std::wstring oldName, unsigned long long processId);
	
	void ForwardEvents(std::future<void> futureObj);
	//void ForwardEvents();
};

