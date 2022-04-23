#include "FuncTool.h"




std::vector<std::string> FuncTool::volumeToPaths(__in PWCHAR VolumeName)
{
    DWORD  CharCount = MAX_PATH + 1;
    PWCHAR Names = NULL;
    PWCHAR NameIdx = NULL;
    BOOL   Success = FALSE;
    std::vector<std::string> rePaths;
    for (;;)
    {
        //
        //  Allocate a buffer to hold the paths.
        Names = (PWCHAR) new BYTE[CharCount * sizeof(WCHAR)];

        if (!Names)
        {
            //
            //  If memory can't be allocated, return.
            return rePaths;
        }

        //
        //  Obtain all of the paths
        //  for this volume.
        Success = GetVolumePathNamesForVolumeNameW(
            VolumeName, Names, CharCount, &CharCount
        );

        if (Success)
        {
            break;
        }

        if (GetLastError() != ERROR_MORE_DATA)
        {
            break;
        }

        //
        //  Try again with the
        //  new suggested size.
        delete[] Names;
        Names = NULL;
    }

    if (Success)
    {
        //
        //  Display the various paths.
        for (NameIdx = Names;
            NameIdx[0] != L'\0';
            NameIdx += wcslen(NameIdx) + 1)
        {
            std::wstring wtmp(NameIdx);
            std::string tmp(wtmp.begin(), wtmp.end());
            rePaths.push_back(tmp);
        }
    }

    if (Names != NULL)
    {
        delete[] Names;
        Names = NULL;
    }
    return rePaths;
}


 std::vector<std::wstring> FuncTool::charArray2VecStr(wchar_t* &data, wchar_t splitKw, int stringBytes)
{
	std::vector<std::wstring> result;
	wchar_t* tmp = new wchar_t[(stringBytes/2)+1];
	for (int i = 0, j = 0; i < stringBytes && result.size() < 2; i++, j++)
	{		
		if ((data[i] == splitKw) || (i == stringBytes))
		{
			tmp[j] = L'\0';
			//printf("split resut: %ls\n", tmp);
			result.push_back(std::wstring(tmp));
			memset(tmp, 0, stringBytes + 1);
			j = -1;
		}
		else 
		{
			tmp[j] = data[i];
		} 
	}
    return result;
}

 std::string FuncTool::convertPathToVolume(std::string p) 
 {
     DWORD  CharCount = 0;
     WCHAR  DeviceName[MAX_PATH] = L"";
     DWORD  Error = ERROR_SUCCESS;
     HANDLE FindHandle = INVALID_HANDLE_VALUE;
     BOOL   Found = FALSE;
     size_t Index = 0;
     BOOL   Success = FALSE;
     WCHAR  VolumeName[MAX_PATH] = L"";
     BOOL finded = FALSE;

     //
     //  Enumerate all volumes in the system.
     FindHandle = FindFirstVolumeW(VolumeName, ARRAYSIZE(VolumeName));

     if (FindHandle == INVALID_HANDLE_VALUE)
     {
         Error = GetLastError();
         wprintf(L"FindFirstVolumeW failed with error code %d\n", Error);
         return 0;
     }

     for (;;)
     {
         //
         //  Skip the \\?\ prefix and remove the trailing backslash.
         Index = wcslen(VolumeName) - 1;

         if (VolumeName[0] != L'\\' ||
             VolumeName[1] != L'\\' ||
             VolumeName[2] != L'?' ||
             VolumeName[3] != L'\\' ||
             VolumeName[Index] != L'\\')
         {
             Error = ERROR_BAD_PATHNAME;
             wprintf(L"FindFirstVolumeW/FindNextVolumeW returned a bad path: %s\n", VolumeName);
             break;
         }
         //
        //  QueryDosDeviceW does not allow a trailing backslash,
        //  so temporarily remove it.
         VolumeName[Index] = L'\0';

         CharCount = QueryDosDeviceW(&VolumeName[4], DeviceName, ARRAYSIZE(DeviceName));

         VolumeName[Index] = L'\\';

         if (CharCount == 0)
         {
             Error = GetLastError();
             wprintf(L"QueryDosDeviceW failed with error code %d\n", Error);
             break;
         }

         //wprintf(L"\nFound a device: %s", DeviceName);
         //wprintf(L"\nVolume name: %s", VolumeName);
         //wprintf(L"\nPaths:");
         std::vector<std::string> t = FuncTool::volumeToPaths(VolumeName);

         for (int i = 0; i < t.size(); i++) 
         {
            int idx = -1;
            if ((idx = p.find(t[i])) != std::string::npos)
            {
                std::wstring wtmp(DeviceName);
                std::string tmp(wtmp.begin(), wtmp.end());
                
                int len = tmp.length();
                if (tmp[len-1] != '\\')
                {
                    tmp.append("\\");
                }
                p.replace(idx, t[i].length(), tmp);

                //int idy = 0;

                //while (((idy = p.find('\\', idy)) != std::string::npos) && p[idy+1]!='\\')
                //{
                //    p.replace(idy, 1, "\\\\");
                //    idy += 2;
                //}
                finded = TRUE;
            }
         }

         if (finded) 
         {
             break;
         }

         //
         //  Move on to the next volume.
         Success = FindNextVolumeW(FindHandle, VolumeName, ARRAYSIZE(VolumeName));

         if (!Success)
         {
             Error = GetLastError();

             if (Error != ERROR_NO_MORE_FILES)
             {
                 wprintf(L"FindNextVolumeW failed with error code %d\n", Error);
                 break;
             }

             //
             //  Finished iterating
             //  through all the volumes.
             Error = ERROR_SUCCESS;
             break;
         }
     }

     FindVolumeClose(FindHandle);
     FindHandle = INVALID_HANDLE_VALUE;
     return p;
 
 
 
 }


