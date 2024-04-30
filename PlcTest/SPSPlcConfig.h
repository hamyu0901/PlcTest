#pragma once

class CSPSPlcConfig
{
public:
	CSPSPlcConfig(void);
	~CSPSPlcConfig(void);

	CSPSPlcConfig(const CSPSPlcConfig & source);
	CSPSPlcConfig & operator = (const CSPSPlcConfig & source);
	BOOL operator () (const CSPSPlcConfig & source);
	BOOL operator == (const CSPSPlcConfig & source);
	void ClearConfig(void);

	// DB Insert 용 배열 string
	CString GetPlcIpString();

	UINT m_uZoneID;											// ZoneID
	bool m_bUsePlcMonitoring;								// 모니터링 여부
	bool m_bUsePlcAutoReconnect;							// 자동 재접속 기능 사용 여부
	int m_nPlcType;											// PLC Type
	CString m_strPlcIpAddr[20];								// IP 주소
	int m_nPlcCpuType;
	int m_nPcStationNo;
	int m_nPlcStationNo;
	int m_nPlcNetworkNo;

	int m_nLocationNo;
};

