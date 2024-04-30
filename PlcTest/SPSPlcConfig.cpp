#include "pch.h"
#include "SPSPlcConfig.h"

CSPSPlcConfig::CSPSPlcConfig(void)
{
	ClearConfig();
}


CSPSPlcConfig::~CSPSPlcConfig(void)
{
}


CSPSPlcConfig::CSPSPlcConfig(const CSPSPlcConfig & source)
{
	if( this == (&source) )
	{
		return;
	}

	(*this) = source;
}


CSPSPlcConfig & CSPSPlcConfig::operator = (const CSPSPlcConfig & source)
{
	m_uZoneID					= source.m_uZoneID;
	m_bUsePlcMonitoring			= source.m_bUsePlcMonitoring;
	m_bUsePlcAutoReconnect		= source.m_bUsePlcAutoReconnect;
	m_nPlcType					= source.m_nPlcType;

	for (int nIdx = 0; nIdx < 20; nIdx++)
		m_strPlcIpAddr[nIdx]	= source.m_strPlcIpAddr[nIdx];

	m_nPlcCpuType				= source.m_nPlcCpuType;
	m_nPcStationNo				= source.m_nPcStationNo;
	m_nPlcStationNo				= source.m_nPlcStationNo;
	m_nPlcNetworkNo				= source.m_nPlcNetworkNo;

	m_nLocationNo				= source.m_nLocationNo;

	return *this;
}


BOOL CSPSPlcConfig::operator () (const CSPSPlcConfig & source)
{
	return (*this == source);
}


BOOL CSPSPlcConfig::operator == (const CSPSPlcConfig & source)
{
	if ( (source.m_uZoneID == m_uZoneID) &&
		(source.m_bUsePlcMonitoring == m_bUsePlcMonitoring) &&
		(source.m_bUsePlcAutoReconnect == m_bUsePlcAutoReconnect) &&
		(source.m_nPlcType == m_nPlcType) &&
		(source.m_strPlcIpAddr[0] == m_strPlcIpAddr[0]) &&
		(source.m_strPlcIpAddr[1] == m_strPlcIpAddr[1]) &&
		(source.m_strPlcIpAddr[2] == m_strPlcIpAddr[2]) &&
		(source.m_strPlcIpAddr[3] == m_strPlcIpAddr[3]) &&
		(source.m_strPlcIpAddr[4] == m_strPlcIpAddr[4]) &&
		(source.m_strPlcIpAddr[5] == m_strPlcIpAddr[5]) &&
		(source.m_strPlcIpAddr[6] == m_strPlcIpAddr[6]) &&
		(source.m_strPlcIpAddr[7] == m_strPlcIpAddr[7]) &&
		(source.m_strPlcIpAddr[8] == m_strPlcIpAddr[8]) &&
		(source.m_strPlcIpAddr[9] == m_strPlcIpAddr[9]) &&
		(source.m_strPlcIpAddr[10] == m_strPlcIpAddr[10]) &&
		(source.m_strPlcIpAddr[11] == m_strPlcIpAddr[11]) &&
		(source.m_strPlcIpAddr[12] == m_strPlcIpAddr[12]) &&
		(source.m_strPlcIpAddr[13] == m_strPlcIpAddr[13]) &&
		(source.m_strPlcIpAddr[14] == m_strPlcIpAddr[14]) &&
		(source.m_strPlcIpAddr[15] == m_strPlcIpAddr[15]) &&
		(source.m_strPlcIpAddr[16] == m_strPlcIpAddr[16]) &&
		(source.m_strPlcIpAddr[17] == m_strPlcIpAddr[17]) &&
		(source.m_strPlcIpAddr[18] == m_strPlcIpAddr[18]) &&
		(source.m_strPlcIpAddr[19] == m_strPlcIpAddr[19]) &&
		(source.m_nPlcCpuType == m_nPlcCpuType) &&
		(source.m_nPcStationNo == m_nPcStationNo) &&
		(source.m_nPlcStationNo == m_nPlcStationNo) &&
		(source.m_nPlcNetworkNo == m_nPlcNetworkNo) &&
		(source.m_nLocationNo == m_nLocationNo) )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


void CSPSPlcConfig::ClearConfig()
{
	m_uZoneID				= 0;
	m_bUsePlcMonitoring		= false;
	m_bUsePlcAutoReconnect	= false;
	m_nPlcType				= PLCTYPE_MELSEC;

	for (int nIdx = 0; nIdx < 20; nIdx++)
		m_strPlcIpAddr[nIdx] = _T("0.0.0.0");

	m_nPlcCpuType			= PLC_CPU_TYPE_12H;
	m_nPcStationNo			= 40;
	m_nPlcStationNo			= 1;
	m_nPlcNetworkNo			= 10;

	m_nLocationNo			= 1;
}


CString CSPSPlcConfig::GetPlcIpString()
{
	CString strPlcIp = _T(""), strTemp = _T("");
	for ( int nIdx = 0; nIdx < 20; nIdx++ )
	{
		if ( m_strPlcIpAddr[nIdx].IsEmpty() == true )
			break;

		if ( strPlcIp.IsEmpty() == true )
		{
			strPlcIp.Format( _T("%s"), m_strPlcIpAddr[nIdx] );
		}
		else
		{
			strTemp.Format( _T(", %s"), m_strPlcIpAddr[nIdx] );
			strPlcIp += strTemp;
		}
	}

	return strPlcIp;
}
