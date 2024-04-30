#pragma once

#include "SPSPlcConfig.h"

enum PLC_STATUS
{
	PLC_STATUS_NOT_USE = 0,
	PLC_STATUS_OFFLINE,
	PLC_STATUS_ONLINE,	
	PLC_STATUS_MAX
};

class CPLCMonitoring
{
public:
	CPLCMonitoring(void);
	~CPLCMonitoring(void);

	BOOL Initialize(CSystemLog* pSystemLog, CSPSPlcConfig plcConfig, CDCAddressMap dcAddressMap);
	void DeInitialize(void);
	
	BOOL OpenCommModule(void);
	BOOL CloseCommModule(void);

	BOOL IsConnect(void);
	BOOL IsConnect(int nIndex);

	BOOL ReadDeviceBlock(CString & strDevice, int nStartAddress, int nDataCount, SHORT ** pReadArrayWord, UINT uRobotNo = 99);
	BOOL WriteDeviceBlock(CString & strDevice, int nStartAddress, int nDataCount, SHORT * pWriteArrayWord, UINT uRobotNo = 99);

	BOOL ConnectPlc();
	BOOL DisconnectPlc();

	// Historian Data
	BOOL ReadHistorianData(map<int, vector<int>> &mapData);
	bool ProcessingHistorianData(SHORT* pBuff, int nReadSize, map<int, vector<int>> &mapData);
	bool ProcessingHistorianDataForSPC(SHORT* pBuff, int nReadSize, int nRobotIndex, map<int, vector<int>> &mapData);

	BOOL WriteHistorianItemData(vector<SHORT> vecData);
	BOOL WriteHistorianDoneData(vector<SHORT> vecData);

	// Monitoring Data
//	BOOL ReadPlcMonitoringData();
//	bool ProcessingMonitoringData(SHORT* pBuff, int nReadSize, int nRobotIndex = 99);
	
	int GetFirstDonePosition(int nDStartAddr, int nMStartAddr);
	bool GetWriteInfoOfDoneAddress(CString strSrcDevice, int nSrcDStartAddr, int nOffset, CString & strDesDevice, int & nDesDStartAddr);

private:
	CDCAddressMap							m_dcAddressMap;				// Historian 데이터 저장 Address
	CSPSPlcConfig							m_plcConfig;

	CDRDriver								m_IODriver;
	CDriverConfig							m_obDriverConfig;

	BOOL									m_bIsConnected;
	int										m_nReadFailCount;

	// Embedded 관련
	map<int, CDRDriver>						m_mapEmbeddedDriver;
	map<int, CDriverConfig>					m_mapEmbeddedConfig;
	
	// Historian 데이터
	vector<vector<int>>						m_vecHistorian;
	std::mutex								m_mutexHistorian;

	int										m_nPLCOpenFailCount;

	CSystemLog*								m_pSystemLog;
};
