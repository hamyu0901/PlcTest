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

	// DB Insert �� �迭 string
	CString GetPlcIpString();

	UINT m_uZoneID;											// ZoneID
	bool m_bUsePlcMonitoring;								// ����͸� ����
	bool m_bUsePlcAutoReconnect;							// �ڵ� ������ ��� ��� ����
	int m_nPlcType;											// PLC Type
	CString m_strPlcIpAddr[20];								// IP �ּ�
	int m_nPlcCpuType;
	int m_nPcStationNo;
	int m_nPlcStationNo;
	int m_nPlcNetworkNo;

	int m_nLocationNo;
};

