#include "pch.h"
#include "PLCMonitoring.h"


CPLCMonitoring::CPLCMonitoring(void)
	: m_nPLCOpenFailCount(0)
	, m_bIsConnected(FALSE)
	, m_nReadFailCount(0)
{
}


CPLCMonitoring::~CPLCMonitoring(void)
{
}


BOOL CPLCMonitoring::Initialize(CSystemLog* pSystemLog, CSPSPlcConfig plcConfig, CDCAddressMap dcAddressMap)
{
	m_pSystemLog = pSystemLog;
	m_plcConfig = plcConfig;
	m_dcAddressMap = dcAddressMap;

	CString strLog;

	switch ( m_plcConfig.m_nPlcType )
	{
	case PLCTYPE_MELSEC:
		m_obDriverConfig.m_iMelsec_LogicalStationNumber = m_plcConfig.m_nLocationNo;
		break;

	case PLCTYPE_SIEMENS:
		m_obDriverConfig.m_strIPAddress					= m_plcConfig.m_strPlcIpAddr[0];
		m_obDriverConfig.m_iSiemens_RackNumber			= m_plcConfig.m_nPcStationNo;
		m_obDriverConfig.m_iSiemens_SlotNumber			= m_plcConfig.m_nPlcStationNo;
		break;

	case PLCTYPE_AB:
		m_obDriverConfig.m_strIPAddress					= m_plcConfig.m_strPlcIpAddr[0];
		m_obDriverConfig.m_iAB_ConnSerialNo				= m_plcConfig.m_nPcStationNo;
		m_obDriverConfig.m_iAB_Slot						= m_plcConfig.m_nPlcStationNo;
		break;

	case PLCTYPE_EMBEDDED:
		{
			//if (pDCConfig->m_mapAddrMap.size() > 0)
			{
				BOOL bResult = TRUE;
				m_mapEmbeddedDriver.clear();
				m_mapEmbeddedConfig.clear();

				// Embedded 드라이버 init
				for ( int nIdx = 0; nIdx < 20; nIdx++ )
				{
					if ( m_plcConfig.m_strPlcIpAddr[nIdx] == _T("0.0.0.0") || m_plcConfig.m_strPlcIpAddr[nIdx].IsEmpty() )
						continue;

					CDRDriver driver;
					CDriverConfig config;

					config.m_strIPAddress = m_plcConfig.m_strPlcIpAddr[nIdx];
					config.m_uPort = m_plcConfig.m_nPcStationNo;

					if ( driver.Initialize(PLCTYPE_EMBEDDED, pSystemLog, &config) == FALSE )
					{
						strLog.Format( _T("CPLCMonitoring::Initialize() - [SPS Index : %d] Initialize failed! (%s)"), nIdx, m_IODriver.GetLastErrorMessage() );
						if ( m_pSystemLog != nullptr )
							m_pSystemLog->AddLog( COLLECTOR_NAME, strLog, LOG_LEVEL_WARNING_GUI );
					}

					LONG lReturnCode;
					bResult &= driver.Open(&lReturnCode);

					m_mapEmbeddedConfig.insert( make_pair(nIdx, config) );
					m_mapEmbeddedDriver.insert( make_pair(nIdx, driver) );
				}

				return bResult;
			}
			break;
		}

	case PLCTYPE_LS:
		m_obDriverConfig.m_strIPAddress = m_plcConfig.m_strPlcIpAddr[0];
		m_obDriverConfig.m_uPort = 2004;
		break;
	}

	if ( m_plcConfig.m_nPlcType != PLCTYPE_EMBEDDED )
	{
		if ( m_IODriver.Initialize(m_plcConfig.m_nPlcType, pSystemLog, &(m_obDriverConfig)) == FALSE )
		{
			strLog.Format( _T("CPLCMonitoring::Initialize() - Initialize failed! (%s)"), m_IODriver.GetLastErrorMessage() );
			if ( m_pSystemLog != nullptr )
				m_pSystemLog->AddLog( COLLECTOR_NAME, strLog, LOG_LEVEL_WARNING_GUI );

			return FALSE;
		}

		if ( OpenCommModule() == FALSE )
			return FALSE;
	}

	return TRUE;
}


void CPLCMonitoring::DeInitialize(void)
{
	if ( m_plcConfig.m_nPlcType == PLCTYPE_EMBEDDED )
	{
		LONG lReturnCode = 0;

		// Embedded 드라이버 Deinit
		for ( auto itEmbedded : m_mapEmbeddedDriver )
		{
			itEmbedded.second.Close(&lReturnCode);
			itEmbedded.second.Deinitialize();
		}
	}
	else
	{
		CloseCommModule();

		m_IODriver.Deinitialize();
	}

	m_mapEmbeddedDriver.clear();
	m_mapEmbeddedConfig.clear();
	
	//if (AfxGetApp()->GetMainWnd()->GetSafeHwnd() != nullptr)
	//	AfxGetApp()->GetMainWnd()->PostMessage(UM_PLC_STATUS_CHANGED, (WPARAM)m_uZoneID, (LPARAM)PLC_STATUS_OFFLINE);
}


BOOL CPLCMonitoring::OpenCommModule(void)
{
	LONG lReturnCode = 0;
	CString strLogMessage;

	if ( m_IODriver.Open(&lReturnCode) == TRUE )
	{
		m_bIsConnected = TRUE;

		strLogMessage.Format( _T("CPLCMonitoring::OpenCommModule() - success!") );
		if ( m_pSystemLog != nullptr )
			m_pSystemLog->AddLog( COLLECTOR_NAME, strLogMessage, LOG_LEVEL_NORMAL_GUI );
	}
	else
	{
		strLogMessage.Format( _T("CPLCMonitoring::OpenCommModule() - open failed! (%s)"), m_IODriver.GetErrorMessage(lReturnCode) );
		if ( m_pSystemLog != nullptr )
			m_pSystemLog->AddLog( COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI );
		
		m_bIsConnected = FALSE;
	}

	return m_bIsConnected;
}


BOOL CPLCMonitoring::CloseCommModule(void)
{
	LONG lReturnCode = 0;
	CString strLogMessage;

	if ( m_bIsConnected == TRUE )
	{
		m_IODriver.Close(&lReturnCode);
		m_bIsConnected = FALSE;
		m_nReadFailCount = 0;

		strLogMessage.Format( _T("CPLCMonitoring::CloseCommModule() - complete!") );
		if ( m_pSystemLog != nullptr )
			m_pSystemLog->AddLog( COLLECTOR_NAME, strLogMessage, LOG_LEVEL_NORMAL_GUI );
	}

	return TRUE;
}


BOOL CPLCMonitoring::IsConnect()
{
	BOOL bResult = TRUE;

	if ( m_plcConfig.m_nPlcType == PLCTYPE_EMBEDDED )
	{
		if ( m_mapEmbeddedDriver.size() <= 0 )
			bResult = FALSE;

		for ( auto & it : m_mapEmbeddedDriver )
		{
			if ( it.second.IsConnect() == FALSE )
			{
				bResult = FALSE;
				break;
			}
		}
	}
	else
	{
		bResult = m_bIsConnected;
	}

	return bResult;
}


BOOL CPLCMonitoring::IsConnect(int nIndex)
{
	BOOL bResult = TRUE;

	if (m_plcConfig.m_nPlcType == PLCTYPE_EMBEDDED)
	{
		if (m_mapEmbeddedDriver.size() <= 0)
			bResult = FALSE;

		for (auto & it : m_mapEmbeddedDriver)
		{
			if (it.first == nIndex)
			{
				return it.second.IsConnect();
			}
		}

		bResult = FALSE;
	}

	return bResult;
}


BOOL CPLCMonitoring::ReadDeviceBlock(CString & strDevice, int nStartAddress, int nDataCount, SHORT ** pReadArrayWord, UINT uRobotNo)
{
	BOOL bRet = FALSE;
	LONG lReturnCode = 0;
	CString strErrorMsg;

	// Embedded 인 경우
	if ( uRobotNo != 99 )
	{
		auto itEmbedded = m_mapEmbeddedDriver.find(uRobotNo);
		if ( itEmbedded != m_mapEmbeddedDriver.end() )
		{
			if ( itEmbedded->second.IsConnect() == TRUE )
			{
				bRet = itEmbedded->second.ReadData( strDevice, nStartAddress, nDataCount, *pReadArrayWord, &lReturnCode );
			}
		}
	}
	else
	{
		bRet = m_IODriver.ReadData( strDevice, nStartAddress, nDataCount, *pReadArrayWord, &lReturnCode );
	}

	if ( bRet == FALSE )
	{
		strErrorMsg.Format( _T("CPLCMonitoring::ReadDeviceBlock() - read error : %s (0x%02X)"), m_IODriver.GetLastErrorMessage(), lReturnCode );
		if ( m_pSystemLog != nullptr )
			m_pSystemLog->AddLog( COLLECTOR_NAME, strErrorMsg, LOG_LEVEL_WARNING_GUI );
	}

	return bRet;
}


BOOL CPLCMonitoring::WriteDeviceBlock(CString & strDevice, int nStartAddress, int nDataCount, SHORT * pWriteArrayWord, UINT uRobotNo)
{
	BOOL bRet = FALSE;
	LONG lReturnCode;
	CString strErrorMsg;

	// Embedded 인 경우
	if ( uRobotNo != 99 )
	{
		auto itEmbedded = m_mapEmbeddedDriver.find(uRobotNo);
		if ( itEmbedded != m_mapEmbeddedDriver.end() )
		{
			if ( itEmbedded->second.IsConnect() == TRUE )
			{
				bRet = itEmbedded->second.WriteData( strDevice, nStartAddress, nDataCount, pWriteArrayWord, &lReturnCode );
			}
		}
	}
	else
	{
		bRet = m_IODriver.WriteData( strDevice, nStartAddress, nDataCount, pWriteArrayWord, &lReturnCode );
	}

	if ( bRet == FALSE )
	{
		strErrorMsg.Format( _T("CPLCMonitoring::WriteDeviceBlock() - write error : %s (0x%02X)"), m_IODriver.GetLastErrorMessage(), lReturnCode );
		if ( m_pSystemLog != nullptr )
			m_pSystemLog->AddLog( COLLECTOR_NAME, strErrorMsg, LOG_LEVEL_WARNING_GUI );
	}

	return bRet;
}


BOOL CPLCMonitoring::ConnectPlc()
{
	BOOL bResult = TRUE;
	CString strLog = _T("");

	if ( m_plcConfig.m_nPlcType == PLCTYPE_EMBEDDED )
	{
		for ( auto & it : m_mapEmbeddedDriver )
		{
			if ( it.second.IsConnect() == FALSE )
			{
				long lReturnCode = 0;

				it.second.Close(&lReturnCode);
				it.second.Deinitialize();

				auto &itConfig = m_mapEmbeddedConfig.find( it.first );
				if ( itConfig != m_mapEmbeddedConfig.end() )
				{
					if ( it.second.Initialize(PLCTYPE_EMBEDDED, m_pSystemLog, &itConfig->second) == FALSE )
					{
						strLog.Format( _T("CPLCMonitoring::ConnectPlc() - IPC(No : %d) initialize failed!"), it.first );
						if ( m_pSystemLog != nullptr )
							m_pSystemLog->AddLog( COLLECTOR_NAME, strLog, LOG_LEVEL_WARNING_GUI );

						bResult = FALSE;
					}

					if ( it.second.Open(&lReturnCode) == FALSE )
					{
						strLog.Format( _T("CPLCMonitoring::ConnectPlc() - IPC(No : %d) open failed!"), it.first );
						if ( m_pSystemLog != nullptr )
							m_pSystemLog->AddLog( COLLECTOR_NAME, strLog, LOG_LEVEL_WARNING_GUI );

						bResult = FALSE;
					}
				}
			}
		}
	}
	else
	{
		if ( m_bIsConnected == TRUE )
			CloseCommModule();

		strLog.Format( _T("CPLCMonitoring::ConnectPlc() - PLC Open Try : %d"), ++m_nPLCOpenFailCount );
		if ( m_pSystemLog != nullptr )
			m_pSystemLog->AddLog( COLLECTOR_NAME, strLog, LOG_LEVEL_NORMAL_GUI );

		if ( OpenCommModule() == TRUE )
		{
			m_nPLCOpenFailCount = 0;

			strLog.Format( _T("CPLCMonitoring::ConnectPlc() - PLC Open Success!") );
			if ( m_pSystemLog != nullptr )
				m_pSystemLog->AddLog( COLLECTOR_NAME, strLog, LOG_LEVEL_NORMAL_GUI );

			bResult = TRUE;
		}
		else
			bResult = FALSE;
	}

	return bResult;
}


BOOL CPLCMonitoring::DisconnectPlc()
{
	BOOL bResult = TRUE;
	CString strLog = _T("");

	if ( m_plcConfig.m_nPlcType == PLCTYPE_EMBEDDED )
	{
		for ( auto & it : m_mapEmbeddedDriver )
		{
			if ( it.second.IsConnect() == TRUE )
			{
				long lReturnCode = 0;

				it.second.Close(&lReturnCode);
				it.second.Deinitialize();
			}
		}
	}
	else
	{
		if ( m_bIsConnected == TRUE )
			bResult = CloseCommModule();
	}

	return bResult;
}


BOOL CPLCMonitoring::WriteHistorianItemData(vector<SHORT> vecData)
{
	BOOL bRet = TRUE;
	int nWriteSize = 0;
	CString strDevice = m_dcAddressMap.m_strDevice;

	if (m_plcConfig.m_nPlcType == PLCTYPE_SIEMENS)
		nWriteSize = (m_dcAddressMap.m_nMEndAddr - m_dcAddressMap.m_nMStartAddr) / 2 + 1;
	else
		nWriteSize = m_dcAddressMap.m_nMEndAddr - m_dcAddressMap.m_nMStartAddr + 1;

	if (nWriteSize <= 0)
		return FALSE;

	switch (m_plcConfig.m_nPlcType)
	{
	case PLCTYPE_EMBEDDED:
	{
		for (int nIdx = 0; nIdx < m_dcAddressMap.m_nDoneCount; nIdx++)
		{
			if (IsConnect(nIdx) == FALSE)
				continue;

			SHORT* pWriteBuf = new SHORT[nWriteSize];
			::ZeroMemory(pWriteBuf, sizeof(SHORT) * nWriteSize);

			for (int nDataIdx = 0; nDataIdx < nWriteSize; nDataIdx++)
			{
				if ( ((nIdx * nWriteSize) + nDataIdx) < vecData.size() )
					pWriteBuf[nDataIdx] = vecData.at((nIdx * nWriteSize) + nDataIdx);
				else
					pWriteBuf[nDataIdx] = 0;
			}

			if (WriteDeviceBlock(strDevice, m_dcAddressMap.m_nMStartAddr, nWriteSize, pWriteBuf, nIdx) == FALSE)
			{
				bRet = FALSE;
				if (m_pSystemLog != nullptr)
					m_pSystemLog->AddLog(COLLECTOR_NAME, _T("Write Histoian Data Fail!"), LOG_LEVEL_WARNING_GUI);
			}

			if (pWriteBuf)
			{
				delete[] pWriteBuf;
				pWriteBuf = nullptr;
			}
		}
	}
	break;
	case PLCTYPE_AB:
	case PLCTYPE_SIEMENS:
	{
		SHORT* pWriteBuf = new SHORT[nWriteSize];
		::ZeroMemory(pWriteBuf, sizeof(SHORT) * nWriteSize);

		copy(vecData.begin(), vecData.end(), pWriteBuf);

		if (WriteDeviceBlock(strDevice, m_dcAddressMap.m_nMStartAddr, nWriteSize, pWriteBuf) == FALSE)
		{
			bRet = FALSE;
			if (m_pSystemLog != nullptr)
				m_pSystemLog->AddLog(COLLECTOR_NAME, _T("Write Histoian Data Fail!"), LOG_LEVEL_WARNING_GUI);
		}

		if (pWriteBuf)
		{
			delete[] pWriteBuf;
			pWriteBuf = nullptr;
		}
	}
	break;
	case PLCTYPE_MELSEC:
	case PLCTYPE_LS:
	{
		strDevice.Format(_T("%s%d"), m_dcAddressMap.m_strDevice, m_dcAddressMap.m_nMStartAddr);

		SHORT* pWriteBuf = new SHORT[nWriteSize];
		::ZeroMemory(pWriteBuf, sizeof(SHORT) * nWriteSize);

		copy(vecData.begin(), vecData.end(), pWriteBuf);

		if (WriteDeviceBlock(strDevice, m_dcAddressMap.m_nMStartAddr, nWriteSize, pWriteBuf) == FALSE)
		{
			bRet = FALSE;
			if (m_pSystemLog != nullptr)
				m_pSystemLog->AddLog(COLLECTOR_NAME, _T("Write Histoian Data Fail!"), LOG_LEVEL_WARNING_GUI);
		}

		if (pWriteBuf)
		{
			delete[] pWriteBuf;
			pWriteBuf = nullptr;
		}
	}
	break;
	default:
		return FALSE;
	}

	return bRet;
}


BOOL CPLCMonitoring::WriteHistorianDoneData(vector<SHORT> vecData)
{
	BOOL bRet = TRUE;
	int nWriteSize = 0;
	CString strDevice = m_dcAddressMap.m_strDevice;

	if (m_plcConfig.m_nPlcType == PLCTYPE_SIEMENS)
		nWriteSize = (m_dcAddressMap.m_nDEndAddr - m_dcAddressMap.m_nDStartAddr) / 2 + 1;
	else
		nWriteSize = m_dcAddressMap.m_nDEndAddr - m_dcAddressMap.m_nDStartAddr + 1;

	if (nWriteSize <= 0)
		return FALSE;

	switch (m_plcConfig.m_nPlcType)
	{
	case PLCTYPE_EMBEDDED:
	{
		for (int nIdx = 0; nIdx < m_dcAddressMap.m_nDoneCount; nIdx++)
		{
			if (IsConnect(nIdx) == FALSE)
				continue;

			SHORT sWriteBuf = 0;

			if (vecData.size() > nIdx)
				sWriteBuf = vecData.at(nIdx);

			if (WriteDeviceBlock(strDevice, m_dcAddressMap.m_nDStartAddr, 1, &sWriteBuf, nIdx) == FALSE)
			{
				bRet = FALSE;
				if (m_pSystemLog != nullptr)
					m_pSystemLog->AddLog(COLLECTOR_NAME, _T("Write Histoian Data Fail!"), LOG_LEVEL_WARNING_GUI);
			}
		}
	}
	break;
	case PLCTYPE_AB:
	case PLCTYPE_SIEMENS:
	{
		SHORT* pWriteBuf = new SHORT[nWriteSize];
		::ZeroMemory(pWriteBuf, sizeof(SHORT) * nWriteSize);

		copy(vecData.begin(), vecData.end(), pWriteBuf);

		if (WriteDeviceBlock(strDevice, m_dcAddressMap.m_nDStartAddr, nWriteSize, pWriteBuf) == FALSE)
		{
			bRet = FALSE;
			if (m_pSystemLog != nullptr)
				m_pSystemLog->AddLog(COLLECTOR_NAME, _T("Write Histoian Data Fail!"), LOG_LEVEL_WARNING_GUI);
		}

		if (pWriteBuf)
		{
			delete[] pWriteBuf;
			pWriteBuf = nullptr;
		}
	}
	break;
	case PLCTYPE_MELSEC:
	case PLCTYPE_LS:
	{
		strDevice.Format(_T("%s%d"), m_dcAddressMap.m_strDevice, m_dcAddressMap.m_nDStartAddr);

		SHORT* pWriteBuf = new SHORT[nWriteSize];
		::ZeroMemory(pWriteBuf, sizeof(SHORT) * nWriteSize);

		copy(vecData.begin(), vecData.end(), pWriteBuf);

		if (WriteDeviceBlock(strDevice, m_dcAddressMap.m_nDStartAddr, nWriteSize, pWriteBuf) == FALSE)
		{
			bRet = FALSE;
			if (m_pSystemLog != nullptr)
				m_pSystemLog->AddLog(COLLECTOR_NAME, _T("Write Histoian Data Fail!"), LOG_LEVEL_WARNING_GUI);
		}

		if (pWriteBuf)
		{
			delete[] pWriteBuf;
			pWriteBuf = nullptr;
		}
	}
	break;
	default:
		return FALSE;
	}

	return bRet;
}


BOOL CPLCMonitoring::ReadHistorianData(map<int, vector<int>> &mapData)
{
	BOOL bRet = FALSE;
	int nReadSize = 0;
	CString strDevice = m_dcAddressMap.m_strDevice;

	if (m_plcConfig.m_nPlcType == PLCTYPE_SIEMENS)
		nReadSize = (m_dcAddressMap.m_nDEndAddr - m_dcAddressMap.m_nMStartAddr) / 2 + 1;
	else
		nReadSize = m_dcAddressMap.m_nDEndAddr - m_dcAddressMap.m_nMStartAddr + 1;

	if (nReadSize <= 0)
		return FALSE;

	switch (m_plcConfig.m_nPlcType)
	{
	case PLCTYPE_EMBEDDED:
	{
		for (int nIdx = 0; nIdx < m_dcAddressMap.m_nDoneCount; nIdx++)
		{
			if (IsConnect(nIdx) == FALSE)
				continue;

			SHORT* pReadBuf = new SHORT[nReadSize];
			::ZeroMemory(pReadBuf, sizeof(SHORT) * nReadSize);
			
			bRet = ReadDeviceBlock(strDevice, m_dcAddressMap.m_nMStartAddr, nReadSize, &pReadBuf, nIdx);
			if (bRet == TRUE)
			{
				ProcessingHistorianDataForSPC(pReadBuf, nReadSize, nIdx, mapData);
			
				m_nReadFailCount = 0;
			}
			else
			{
				// 5번 연속 읽기 실패 시 PLC 연결 해제된것으로 판단
				if (++m_nReadFailCount > 5)
				{
					CString strLogMessage;
					strLogMessage.Format(_T("ReadHistorianData() - ReadDeviceBlock() Fail, set disconnect status! : Device: %s"), strDevice);
					if (m_pSystemLog != nullptr)
						m_pSystemLog->AddLog(COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI);
			
					DisconnectPlc();
				}
			}
		}
	}
	break;
	case PLCTYPE_AB:
	case PLCTYPE_SIEMENS:
	{
		SHORT* pReadBuf = new SHORT[nReadSize];
		::ZeroMemory(pReadBuf, sizeof(SHORT) * nReadSize);

		bRet = ReadDeviceBlock(strDevice, m_dcAddressMap.m_nMStartAddr, nReadSize, &pReadBuf);
		if (bRet == TRUE)
		{
			ProcessingHistorianData(pReadBuf, nReadSize, mapData);

			m_nReadFailCount = 0;
		}
		else
		{
			// 5번 연속 읽기 실패 시 PLC 연결 해제된것으로 판단
			if (++m_nReadFailCount > 5)
			{
				CString strLogMessage;
				strLogMessage.Format(_T("ReadHistorianData() - ReadDeviceBlock() Fail, set disconnect status! : Device: %s"), strDevice);
				if (m_pSystemLog != nullptr)
					m_pSystemLog->AddLog(COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI);

				DisconnectPlc();
			}
		}

		if (pReadBuf)
		{
			delete[] pReadBuf;
			pReadBuf = nullptr;
		}
	}
	break;
	case PLCTYPE_MELSEC:
	case PLCTYPE_LS:
	{
		strDevice.Format(_T("%s%d"), m_dcAddressMap.m_strDevice, m_dcAddressMap.m_nMStartAddr);

		SHORT* pReadBuf = new SHORT[nReadSize];
		::ZeroMemory(pReadBuf, sizeof(SHORT) * nReadSize);

		bRet = ReadDeviceBlock(strDevice, m_dcAddressMap.m_nMStartAddr, nReadSize, &pReadBuf);
		if (bRet == TRUE)
		{
			ProcessingHistorianData(pReadBuf, nReadSize, mapData);

			m_nReadFailCount = 0;
		}
		else
		{
			// 5번 연속 읽기 실패 시 PLC 연결 해제된것으로 판단
			if (++m_nReadFailCount > 5)
			{
				CString strLogMessage;
				strLogMessage.Format(_T("ReadHistorianData() - ReadDeviceBlock() Fail, set disconnect status! : Device: %s"), strDevice);
				if (m_pSystemLog != nullptr)
					m_pSystemLog->AddLog(COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI);

				DisconnectPlc();
			}
		}

		if (pReadBuf)
		{
			delete[] pReadBuf;
			pReadBuf = nullptr;
		}
	}
	break;
	default:
		return FALSE;
	}

	if (bRet == FALSE)
	{
		CString strLogMessage;
		strLogMessage.Format(_T("Historian data Read Fail : Device: %s, Start : %d, Read Size : %d"), strDevice, m_dcAddressMap.m_nMStartAddr, nReadSize);
		if (m_pSystemLog != nullptr)
			m_pSystemLog->AddLog(COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI);
	}

	return bRet;
}


bool CPLCMonitoring::ProcessingHistorianData(SHORT* pBuff, int nReadSize, map<int, vector<int>> &mapData)
{
	int nDonePosition = GetFirstDonePosition(m_dcAddressMap.m_nDStartAddr, m_dcAddressMap.m_nMStartAddr);
	if (nDonePosition < 0)
		return false;

	for (int nIdx = 0; nIdx < m_dcAddressMap.m_nDoneCount; nIdx++)
	{
		vector<int> vecHistorianData;

		//PLC Read
		int nReadAddr = nIdx * m_dcAddressMap.m_nMItemCount;

		for (int j = 0; j < m_dcAddressMap.m_nMItemCount; j++)
		{
			vecHistorianData.push_back(pBuff[nReadAddr++]);
		}

		// Job Done
		vecHistorianData.push_back(pBuff[nDonePosition++]);

		mapData.insert(make_pair(nIdx, vecHistorianData));
	}

	return true;
}


bool CPLCMonitoring::ProcessingHistorianDataForSPC(SHORT* pBuff, int nReadSize, int nRobotIndex, map<int, vector<int>> &mapData)
{
	int nDonePosition = GetFirstDonePosition(m_dcAddressMap.m_nDStartAddr, m_dcAddressMap.m_nMStartAddr);
	if (nDonePosition < 0)
		return false;
	
	vector<int> vecHistorianData;

	//PLC Read
	int nReadAddr = 0;
	for (int j = 0; j < m_dcAddressMap.m_nMItemCount; j++)
	{
		vecHistorianData.push_back(pBuff[nReadAddr++]);
	}

	// Job Done
	vecHistorianData.push_back(pBuff[nDonePosition++]);

	mapData.insert(make_pair(nRobotIndex, vecHistorianData));

	return true;
}


//BOOL CPLCMonitoring::ReadPlcMonitoringData()
//{
//	BOOL bRet = FALSE;
//	int nReadSize = 0;
//	CSPSPlcMonitoringConfig monConfig = m_plcMonitoringConf;
//	CString strDevice = monConfig.m_strDeviceName;
//
//	if ( m_plcConfig.m_nPlcType == PLCTYPE_SIEMENS )
//		nReadSize = (monConfig.m_nEndAddress - monConfig.m_nStartAddress) / 2 + 1;
//	else
//		nReadSize = monConfig.m_nEndAddress - monConfig.m_nStartAddress + 1;
//
//	if ( nReadSize <= 0 )
//		return FALSE;
//
//	switch ( m_plcConfig.m_nPlcType )
//	{
//	case PLCTYPE_EMBEDDED:
//	{
//		// 서로 다른 IPC-IP와 같은 Block Number, Data Address
//		for ( auto itRobot : monConfig.m_mapRobotItemAddress )
//		{
//			//if (itR.bRobotUsable == true)
//			{
//				SHORT* pReadBuf = new SHORT[nReadSize];
//				::ZeroMemory( pReadBuf, sizeof(SHORT) * nReadSize );
//
//				bRet = ReadDeviceBlock( strDevice, monConfig.m_nStartAddress, nReadSize, &pReadBuf, itRobot.first );
//				if ( bRet == TRUE )
//				{
//					ProcessingMonitoringData( pReadBuf, nReadSize, itRobot.first );
//
//					m_nReadFailCount = 0;
//				}
//				else
//				{
//					// 5번 연속 읽기 실패 시 PLC 연결 해제된것으로 판단
//					if ( ++m_nReadFailCount > 5 )
//					{
//						CString strLogMessage;
//						strLogMessage.Format( _T("ReadPlcMonitoringData() - ReadDeviceBlock() Fail, set disconnect status! : Device: %s"), strDevice );
//						if ( m_pSystemLog != nullptr )
//							m_pSystemLog->AddLog( COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI );
//
//						DisconnectPlc();
//					}
//				}
//
//				if ( pReadBuf )
//				{
//					delete[] pReadBuf;
//					pReadBuf = nullptr;
//				}
//			}
//		}
//	}
//	break;
//	case PLCTYPE_AB:
//	case PLCTYPE_SIEMENS:
//	{
//		// 동일한 지멘스-IP와 Data Address, 다른 Block Number
//		for ( auto itRobot : monConfig.m_mapRobotItemAddress )
//		{
//			//if (itRobot.bRobotUsable == true)
//			if ( itRobot.first < (int)monConfig.m_vecDeviceBlockName.size() )
//			{
//				SHORT* pReadBuf = new SHORT[nReadSize];
//				::ZeroMemory( pReadBuf, sizeof(SHORT) * nReadSize );
//				
//				bRet = ReadDeviceBlock( monConfig.m_vecDeviceBlockName.at(itRobot.first), monConfig.m_nStartAddress, nReadSize, &pReadBuf );
//				if ( bRet == TRUE )
//				{
//					ProcessingMonitoringData( pReadBuf, nReadSize, itRobot.first );
//
//					m_nReadFailCount = 0;
//				}
//				else
//				{
//					// 5번 연속 읽기 실패 시 PLC 연결 해제된것으로 판단
//					if ( ++m_nReadFailCount > 5 )
//					{
//						CString strLogMessage;
//						strLogMessage.Format( _T("ReadPlcMonitoringData() - ReadDeviceBlock() Fail, set disconnect status! : Device: %s"), strDevice );
//						if ( m_pSystemLog != nullptr )
//							m_pSystemLog->AddLog( COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI );
//
//						DisconnectPlc();
//					}
//				}
//
//				if ( pReadBuf )
//				{
//					delete[] pReadBuf;
//					pReadBuf = nullptr;
//				}
//			}
//		}
//	}
//	break;
//	case PLCTYPE_MELSEC:
//	case PLCTYPE_LS:
//		{
//			strDevice.Format( _T("%s%d"), monConfig.m_strDeviceName, monConfig.m_nStartAddress );
//
//			SHORT* pReadBuf = new SHORT[nReadSize];
//			::ZeroMemory( pReadBuf, sizeof(SHORT) * nReadSize );
//
//			bRet = ReadDeviceBlock( strDevice, monConfig.m_nStartAddress, nReadSize, &pReadBuf );
//			if ( bRet == TRUE )
//			{
//				ProcessingMonitoringData( pReadBuf, nReadSize, 99 );
//
//				m_nReadFailCount = 0;
//			}
//			else
//			{
//				// 5번 연속 읽기 실패 시 PLC 연결 해제된것으로 판단
//				if ( ++m_nReadFailCount > 5 )
//				{
//					CString strLogMessage;
//					strLogMessage.Format( _T("ReadPlcMonitoringData() - ReadDeviceBlock() Fail, set disconnect status! : Device: %s"), strDevice );
//					if ( m_pSystemLog != nullptr )
//						m_pSystemLog->AddLog( COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI );
//
//					DisconnectPlc();
//				}
//			}
//
//			if ( pReadBuf )
//			{
//				delete[] pReadBuf;
//				pReadBuf = nullptr;
//			}
//		}
//		break;
//	default:
//		return FALSE;
//	}
//
//	if ( bRet == FALSE )
//	{
//		CString strLogMessage;
//		strLogMessage.Format( _T("Atomizer data Read Fail : Device: %s, Done Start : %d, Read Size : %d"), strDevice, monConfig.m_nStartAddress, nReadSize );
//		if ( m_pSystemLog != nullptr )
//			m_pSystemLog->AddLog( COLLECTOR_NAME, strLogMessage, LOG_LEVEL_WARNING_GUI );
//	}
//
//	return bRet;
//}
//
//
//bool CPLCMonitoring::ProcessingMonitoringData(SHORT* pBuff, int nReadSize, int nRobotIndex)
//{
//	// 로봇당 수집 항목 수
//	int nItemCount = m_plcMonitoringConf.m_vecHeaderItem.size();
//	if ( nItemCount <= 0 )
//	{
//		return false;
//	}
//
//	for ( auto itPlcRobot : m_plcMonitoringConf.m_mapRobotItemAddress )
//	{
//		int nIndex = 0, nRobotID = -1;
//		for ( auto itRobotConf : m_vecRobotConfig )
//		{
//			if ( nIndex++ == itPlcRobot.first )
//			{
//				nRobotID = itRobotConf.m_uRobotID;
//				break;
//			}
//		}
//
//		CSPSPlcHistoryData plcHistoryData;
//
//		SYSTEMTIME tmNow;
//		GetLocalTime(&tmNow);
//
//		plcHistoryData.m_strTime.Format( _T("%04d-%02d-%02d %02d:%02d:%02d.%03d"),
//			tmNow.wYear, tmNow.wMonth, tmNow.wDay, tmNow.wHour, tmNow.wMinute, tmNow.wSecond, tmNow.wMilliseconds );
//
//		SHORT sData = 0;
//		for ( auto itAddress : itPlcRobot.second )
//		{
//			if ( itAddress.IsEmpty() == true )
//			{
//				plcHistoryData.m_vecAtomData.emplace_back(0);
//				continue;
//			}
//
//			int nAddress = _ttoi(itAddress) - m_plcMonitoringConf.m_nStartAddress;
//			if ( m_plcConfig.m_nPlcType == PLCTYPE_SIEMENS )
//				nAddress /= 2;
//
//			if ( nAddress < 0 || nAddress >= nReadSize )
//			{
//				plcHistoryData.m_vecAtomData.emplace_back(0);
//				continue;
//			}
//
//			sData = (SHORT)(pBuff[nAddress]);
//			if ( sData < 0 )
//				sData = 0;
//
//			plcHistoryData.m_vecAtomData.emplace_back(sData);
//		}
//
//		// 실시간 도장기 데이터 셋팅
//		{
//			std::lock_guard<std::mutex> g(m_mutexCurrentAtomizerData);
//
//			auto &itCurData = m_mapCurrentAtomizerData.find(nRobotID);
//			if ( itCurData == m_mapCurrentAtomizerData.end() )
//				m_mapCurrentAtomizerData.insert( make_pair(nRobotID, plcHistoryData) );
//			else
//				itCurData->second = plcHistoryData;
//		}
//
//		// 로봇 모니터링 데이터 셋팅
//		map<int, CSPSMonitoringData> mapRobotMonitoringData;
//		GetRobotMonitoringData(mapRobotMonitoringData);
//
//		auto itMonitoringData = mapRobotMonitoringData.find(nRobotID);
//		if ( itMonitoringData != mapRobotMonitoringData.end() )
//		{
//			if ( (itMonitoringData->second.m_nServoOn == SERVO_STATUS_ON) && (IsMasterJob(itMonitoringData->second.m_strJobName) == false) 
//				&& (IsExcludeJob(itMonitoringData->second.m_strJobName) == false) )
//			{
//				// Step No 셋팅
//				plcHistoryData.m_nStepNo = itMonitoringData->second.m_uStepNo;
//
//				// Job 별 도장기 데이터 셋팅
//				SetAtomizerJobHistoryData( nRobotID, itMonitoringData->second.m_strJobName, plcHistoryData );
//
//				if ( itMonitoringData->second.m_nRobotStatusMode == ROBOT_STATUS_RUN )
//				{
//					// 도장기 avg 데이터 셋팅
//					SetAtomizerAvgHistoryData( nRobotID, plcHistoryData );
//				}
//			}
//		}
//	}
//
//	return true;
//}


int CPLCMonitoring::GetFirstDonePosition(int nDStartAddr, int nMStartAddr)
{
	int nDonePosition = 0;

	switch (m_plcConfig.m_nPlcType)
	{
	case PLCTYPE_MELSEC:
	case PLCTYPE_LS:
	case PLCTYPE_AB:
	case PLCTYPE_EMBEDDED:
		nDonePosition = nDStartAddr - nMStartAddr;
		break;

	case PLCTYPE_SIEMENS:
		nDonePosition = (nDStartAddr - nMStartAddr) / 2;
		break;

	default:
		nDonePosition = -1;
		break;
	}

	return nDonePosition;
}


bool CPLCMonitoring::GetWriteInfoOfDoneAddress(CString strSrcDevice, int nSrcDStartAddr, int nOffset, CString & strDesDevice, int & nDesDStartAddr)
{
	switch (m_plcConfig.m_nPlcType)
	{
	case PLCTYPE_MELSEC:
		strDesDevice.Format(_T("%s%d"), strSrcDevice, nSrcDStartAddr + nOffset);
		nDesDStartAddr = 0;
		break;
	case PLCTYPE_LS:
	case PLCTYPE_EMBEDDED:
	case PLCTYPE_AB:
		strDesDevice.Format(_T("%s"), strSrcDevice);
		nDesDStartAddr = nSrcDStartAddr + nOffset;
		break;
	case PLCTYPE_SIEMENS:
		strDesDevice.Format(_T("%s"), strSrcDevice);
		nDesDStartAddr = nSrcDStartAddr + (nOffset * 2);
		break;
	default:
		return false;
		break;
	}

	return true;
}
