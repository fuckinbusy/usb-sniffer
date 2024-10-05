#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "structs.h"
#include "scanner.h"

BOOL AppendDrive(DRIVES *drives, const char driveName);
BOOL FindDrive(DRIVES *drives, const char driveName);
void PrintVolumeInformation(VOLUME_INFORMATION_P pVolumeInformation);
void ScanDrives(int intervalms);