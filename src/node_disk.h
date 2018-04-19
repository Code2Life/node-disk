#pragma once
#include <stdint.h>
#include <vector>

using namespace std;

typedef struct _PARTITION_INFO{
	string volume;
	string totalSize;
	string freeSize;
} PARTITION, *PPARTITION;

typedef struct _DISK_INFO{
	string totalSize;
	string freeSize;
	vector<PARTITION> partition;
} DISK_INFO, *PDISK_INFO;

bool DoGetDiskInfo(DISK_INFO &disk, string path);