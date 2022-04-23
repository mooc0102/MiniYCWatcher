#pragma once
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <unordered_map>

class FuncTool
{
public:
	static std::vector<std::wstring> charArray2VecStr(wchar_t*& data, wchar_t splitKw, int stringBytes);
	static std::vector<std::string> volumeToPaths(__in PWCHAR VolumeName);
	static std::string convertPathToVolume(std::string p);

private:

};

