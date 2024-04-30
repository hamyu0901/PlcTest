
// PlcTestDlg.h: 헤더 파일
//

#pragma once

#define MAX_PLC_IP_COUNT 20

// CPlcTestDlg 대화 상자
class CPlcTestDlg : public CDialogEx
{
// 생성입니다.
public:
	CPlcTestDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PLCTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

public:
	void InitControls();
	bool LoadXMLConfig(void);
	void UpdateSpread();

	void DispHistorianData();

	void SetConnectionStatus(bool bConnected);

	CString	GetRobotNoString(int nNo);
	int	GetRobotNo(CString strRobotNo);

	void SetPlcConfigUI(int nPlcType);
	void SetPlcUpdateInfo(CString strZoneName, CSPSPlcConfig &plcInfo);

	BOOL SetMelsecRegConfig(CString strZoneName, CSPSPlcConfig plcInfo);

	void ConvertStringNameToBuf(const CString & strName, vector<CString>& vecItemName);
	void ConvertBufToStringName(vector<CString>& vecItemName, CString & strName);

public:
	CSystemLog		m_SystemLog;

	CPLCMonitoring	m_plcHandler;

	CDCAddressMap	m_dcAddressMap;

	CComboBox m_cbPlcType;
	CComboBox m_cbPlcCpuType;
	CComboBox m_cbSpcList;
	CIPAddressCtrl m_editPlcIpAddress;
	int m_nPcStationNo;
	int m_nPlcStationNo;
	int m_nPlcNetworkNo;

	CString m_strTempPlcIP[MAX_PLC_IP_COUNT];

	CFpspread m_spreadConfig;


	map<int, vector<int>> m_mapHistorianData;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnPlcConnect();
	DECLARE_EVENTSINK_MAP()
	void AdvanceFpspreadCofig(BOOL AdvanceNext);
	afx_msg void OnCbnSelchangeCbPlcType();
	afx_msg void OnCbnSelchangeCbPlcCpuType();
	afx_msg void OnCbnSelchangeCbSpcList();
	afx_msg void OnBnClickedBtnReadHistorianData();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnWriteHistorainItemData();
	afx_msg void OnBnClickedBtnWriteHistorianDoneData();
	afx_msg void OnCbnDropdownCbSpcList();
};
