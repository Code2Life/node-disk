#pragma once

#include <stdio.h>
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
	vector<PARTITION> partion;
} DISK_INFO, *PDISK_INFO;

class DiskInfo {

public:
	DiskInfo();
	~DiskInfo();
	void GetDiskInfo(DISK_INFO &disk);

};