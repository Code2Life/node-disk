#include "node_disk.h"
#include <windows.h>

using namespace std;

void Wchar_tToString(std::string& szDst, WCHAR * wchar)
{
	WCHAR * wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);
	char *psText;
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	szDst = psText;
	delete[] psText;
};

string DoubleToString(double num)
{
	char str[256];
	sprintf(str, "%.2f GB", num);
	return str;
}

bool DoGetDiskInfo(DISK_INFO &disk_in, std::string path) {
	DWORD disk_count = 0;
	DWORD disk_info = GetLogicalDrives();
	while (disk_info)
	{
		if (disk_info & 1)
		{
			disk_count++;
		}
		disk_info = disk_info >> 1;
	}

	int disk_length = GetLogicalDriveStrings(0, NULL);

	WCHAR * disk_str = new WCHAR[disk_length];
	memset(disk_str, 0, disk_length);

	GetLogicalDriveStrings(disk_length, (LPWSTR)disk_str);

	bool is_query = false;
	if (path.length() > 0) {
		is_query = true;
	}

	int disk_type;
	int si = 0;
	bool fResult;

	unsigned _int64 i64ALL = 0;
	unsigned _int64 i64FreeAll = 0;
	unsigned _int64 i64FreeBytesToCaller;
	unsigned _int64 i64TotalBytes;
	unsigned _int64 i64FreeBytes;

	for (int i = 0; i < disk_length / 4; i++)
	{

		string tempName = "";
		double tempTotal;
		double tempFree;

		disk_type = GetDriveType((LPCWSTR)disk_str + i * 4);
		Wchar_tToString(tempName, disk_str + i * 4);

		if (is_query && tempName[0] != path[0]) continue;

		if (disk_type == DRIVE_FIXED)
		{
			fResult = GetDiskFreeSpaceEx(disk_str + i * 4, (PULARGE_INTEGER)&i64FreeBytesToCaller, (PULARGE_INTEGER)&i64TotalBytes, (PULARGE_INTEGER)&i64FreeBytes);
			if (fResult)
			{
				tempTotal = i64TotalBytes / 1024 / 1024 / 1024.0;
				tempFree = i64FreeBytesToCaller / 1024 / 1024 / 1024.0;

				PARTITION tempPartion;
				tempPartion.totalSize = DoubleToString(tempTotal);
				tempPartion.freeSize = DoubleToString(tempFree);
				tempPartion.volume = tempName;

				disk_in.partition.push_back(tempPartion);
				i64ALL += i64TotalBytes;
				i64FreeAll += i64FreeBytes;
			}
			else {
				return false;
			}
		}
		si += 4;
	}
	disk_in.freeSize = DoubleToString(i64FreeAll / 1024 / 1024 / 1024.0);
	disk_in.totalSize = DoubleToString(i64ALL / 1024 / 1024 / 1024.0);

	delete[] disk_str;
	return true;
}