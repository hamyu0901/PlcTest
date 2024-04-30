// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H

// 여기에 미리 컴파일하려는 헤더 추가
#include "framework.h"

#ifdef _DEBUG
#pragma comment (lib, "DRSystemLoggerd.lib")
#pragma comment (lib, "DRDriverD.lib")
#else
#pragma comment (lib, "DRSystemLogger.lib")
#pragma comment (lib, "DRDriver.lib")
#endif

using namespace std;

#include <vector>
#include <map>
#include <mutex>
#include <ctime>
#include <istream>
#include <iostream>
#include <string>
#include <thread>
#include <experimental/filesystem>
#include <filesystem>
#include "SystemLog.h"
#include "FileManagement.h"
#include "XMLWrapper.h"
#include "DriverConfig.h"
#include "DoolimDriver.h"
#include "DCDefinition.h"
#include "DCAddressMap.h"
#include "Registry.h"
#include "fpspread.h"
#include "PLCMonitoring.h"

#define COLLECTOR_NAME	_T("PLC_Test")

#endif //PCH_H
