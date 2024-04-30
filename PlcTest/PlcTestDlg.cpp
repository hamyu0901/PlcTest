
// PlcTestDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "PlcTest.h"
#include "PlcTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPlcTestDlg 대화 상자



CPlcTestDlg::CPlcTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PLCTEST_DIALOG, pParent)
	, m_nPcStationNo(40)
	, m_nPlcStationNo(1)
	, m_nPlcNetworkNo(10)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int nIdx = 0; nIdx < MAX_PLC_IP_COUNT; nIdx++)
		m_strTempPlcIP[nIdx] = _T("0.0.0.0");
}

void CPlcTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FPSPREAD_COFIG, m_spreadConfig);
	DDX_Control(pDX, IDC_CB_PLC_TYPE, m_cbPlcType);
	DDX_Control(pDX, IDC_CB_PLC_CPU_TYPE, m_cbPlcCpuType);
	DDX_Control(pDX, IDC_CB_SPC_LIST, m_cbSpcList);
	DDX_Control(pDX, IDC_IPADDR_PLC_IP, m_editPlcIpAddress);
	DDX_Text(pDX, IDC_EDIT_PC_STATION_NO, m_nPcStationNo);
	DDX_Text(pDX, IDC_EDIT_PLC_STATION_NO, m_nPlcStationNo);
	DDX_Text(pDX, IDC_EDIT_PLC_NETWORK_NO, m_nPlcNetworkNo);
}

BEGIN_MESSAGE_MAP(CPlcTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PLC_CONNECT, &CPlcTestDlg::OnBnClickedBtnPlcConnect)
	ON_CBN_SELCHANGE(IDC_CB_PLC_TYPE, &CPlcTestDlg::OnCbnSelchangeCbPlcType)
	ON_CBN_SELCHANGE(IDC_CB_PLC_CPU_TYPE, &CPlcTestDlg::OnCbnSelchangeCbPlcCpuType)
	ON_CBN_SELCHANGE(IDC_CB_SPC_LIST, &CPlcTestDlg::OnCbnSelchangeCbSpcList)
	ON_BN_CLICKED(IDC_BTN_READ_HISTORIAN_DATA, &CPlcTestDlg::OnBnClickedBtnReadHistorianData)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_WRITE_HISTORAIN_ITEM_DATA, &CPlcTestDlg::OnBnClickedBtnWriteHistorainItemData)
	ON_BN_CLICKED(IDC_BTN_WRITE_HISTORIAN_DONE_DATA, &CPlcTestDlg::OnBnClickedBtnWriteHistorianDoneData)
	ON_CBN_DROPDOWN(IDC_CB_SPC_LIST, &CPlcTestDlg::OnCbnDropdownCbSpcList)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CPlcTestDlg, CDialogEx)
	ON_EVENT(CPlcTestDlg, IDC_FPSPREAD_COFIG, 1, CPlcTestDlg::AdvanceFpspreadCofig, VTS_BOOL)
END_EVENTSINK_MAP()

// CPlcTestDlg 메시지 처리기

BOOL CPlcTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_SystemLog.Initialize(CFileManagement::GetModulePath(), COLLECTOR_NAME);

	InitControls();
	LoadXMLConfig();
	UpdateSpread();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CPlcTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPlcTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPlcTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPlcTestDlg::InitControls()
{
	// PLC
	CString strIdx;
	int nIndex = 0;
	m_cbPlcType.InsertString(nIndex, _T("Melsec"));
	m_cbPlcType.SetItemData(nIndex++, (DWORD_PTR)PLCTYPE_MELSEC);
	m_cbPlcType.InsertString(nIndex, _T("Siemens"));
	m_cbPlcType.SetItemData(nIndex++, (DWORD_PTR)PLCTYPE_SIEMENS);
	m_cbPlcType.InsertString(nIndex, _T("AB"));
	m_cbPlcType.SetItemData(nIndex++, (DWORD_PTR)PLCTYPE_AB);
	m_cbPlcType.InsertString(nIndex, _T("Embedded(SPC)"));
	m_cbPlcType.SetItemData(nIndex++, (DWORD_PTR)PLCTYPE_EMBEDDED);
	m_cbPlcType.InsertString(nIndex, _T("LS"));
	m_cbPlcType.SetItemData(nIndex++, (DWORD_PTR)PLCTYPE_LS);
	m_cbPlcType.SetCurSel(0);

	m_cbPlcCpuType.InsertString(0, _T("Q12H"));
	m_cbPlcCpuType.InsertString(1, _T("Q13UDH"));
	m_cbPlcCpuType.SetCurSel(0);

	for (int nIdx = 0; nIdx < MAX_PLC_IP_COUNT; nIdx++)
	{
		m_cbSpcList.InsertString(nIdx, GetRobotNoString(nIdx + 1));
	}
	m_cbSpcList.SetCurSel(0);

	SetPlcConfigUI(PLCTYPE_MELSEC);
}


bool CPlcTestDlg::LoadXMLConfig()
{
	CXMLWrapperNode nodeRoot, nodeAddrConfig, nodeMonitoring;
	CString strRootPath, strFileName, strAttrValue;
	CXMLWrapperDocument doc;

	CString strModulePath = CFileManagement::GetModulePath();
	strRootPath.Format(_T("%s"), CFileManagement::GetFileRootPath(strModulePath));
	strFileName.Format(_T("%s%s\\%s\\%s\\%s"), strRootPath, _T("DoolimData"), _T("Paint"), _T("Config"), _T("SPSDataCollectorNew.xml"));
	if (CFileManagement::FileExists(strFileName) == FALSE)
	{
		AfxMessageBox(_T("[DataCollectorNew.xml] is not exist!"), MB_ICONSTOP);
		return false;
	}

	//	XML 문서를 로드한다.
	if (doc.Open(strFileName) == false)
	{
		AfxMessageBox(_T("[DataCollectorNew.xml] load fail!"), MB_ICONSTOP);
		return false;
	}

	//	ROOT Node를 가져온다.
	nodeRoot = doc.GetRootNode();
	if (nodeRoot.IsNull() == true)
	{
		AfxMessageBox(_T("[DataCollectorNew.xml] get root node fail!"), MB_ICONSTOP);

		doc.Close();

		return false;
	}


	// ADDRCONFIG 노드
	nodeAddrConfig = nodeRoot.SelectSingleNode(NODE_ADDR_CONFIG);
	if (nodeAddrConfig.IsNull() == false)
	{
		BOOL bRet = FALSE;

		// PLC Type
		strAttrValue = _T("");
		bRet = nodeAddrConfig.GetNodeAttrValue(ATTR_PLC_TYPE, strAttrValue);
		if (bRet == FALSE)
		{
			strAttrValue = _T("MELSEC_PLC");
		}
		//strAttrValue;

		// AddressMap Node
		CXMLWrapperNode nodeAddrMap;
		nodeAddrMap = nodeAddrConfig.SelectSingleNode(_T("AddressMap"));
		if (nodeAddrMap.IsNull() == false)
		{
			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_USABLE, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("FALSE");
			}
			m_dcAddressMap.m_strUsable = strAttrValue;

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_DEVICE, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("D");
			}
			m_dcAddressMap.m_strDevice = strAttrValue;

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_MONITORING_START_ADDR, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("5700");
			}
			m_dcAddressMap.m_nMStartAddr = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_MONITORING_END_ADDR, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("5771");
			}
			m_dcAddressMap.m_nMEndAddr = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_MONITORING_ITEM_COUNT, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("9");
			}
			m_dcAddressMap.m_nMItemCount = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_MONITORING_ITEM_NAME, strAttrValue);
			ConvertStringNameToBuf(strAttrValue, m_dcAddressMap.m_vecItemName);

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_DONE_START_ADDR, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("5844");
			}
			m_dcAddressMap.m_nDStartAddr = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_DONE_END_ADDR, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("5851");
			}
			m_dcAddressMap.m_nDEndAddr = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_DONE_COUNT, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("8");
			}
			m_dcAddressMap.m_nDoneCount = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_VIN_NO, strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("0");
			}
			m_dcAddressMap.m_uVinno = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(_T("USE_PLC_VINNO"), strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("0");
			}
			m_dcAddressMap.m_bUseVinno = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(_T("VINNO_INDEX"), strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("0");
			}
			m_dcAddressMap.m_uVinnoIdnex = _ttoi(strAttrValue);

			bRet = nodeAddrMap.GetNodeAttrValue(_T("VINNO_COUNT"), strAttrValue);
			if (bRet == FALSE)
			{
				strAttrValue = _T("0");
			}
			m_dcAddressMap.m_uVinnoCount = _ttoi(strAttrValue);
			//////////////////////////////////////////////////////////////////////////

			bRet = nodeAddrMap.GetNodeAttrValue(ATTR_DONE_NAME, strAttrValue);
			ConvertStringNameToBuf(strAttrValue, m_dcAddressMap.m_vecDoneName);
		}
	}
	else
	{
		m_dcAddressMap.m_strDevice = _T("D");
		m_dcAddressMap.m_strUsable = _T("FALSE");
		m_dcAddressMap.m_nMStartAddr = 5700;
		m_dcAddressMap.m_nMEndAddr = 5771;
		m_dcAddressMap.m_nMItemCount = 9;
		m_dcAddressMap.m_vecItemName.clear();
		m_dcAddressMap.m_nDStartAddr = 5844;
		m_dcAddressMap.m_nDEndAddr = 5851;
		m_dcAddressMap.m_nDoneCount = 8;
		m_dcAddressMap.m_uVinno = 0;
		m_dcAddressMap.m_vecDoneName.clear();

		m_dcAddressMap.m_bUseVinno = 0;
		m_dcAddressMap.m_uVinnoIdnex = 0;
		m_dcAddressMap.m_uVinnoCount = 0;
	}

	doc.Close();

	return true;
}


void CPlcTestDlg::UpdateSpread()
{
	UpdateData();

	m_spreadConfig.SetReDraw(FALSE);
	m_spreadConfig.SetText(-999, 0, _T("ITEM NAME"));
	m_spreadConfig.SetMaxCols(m_dcAddressMap.m_nDoneCount);
	m_spreadConfig.SetMaxRows(m_dcAddressMap.m_nMItemCount + 1);

	// Done Name 헤더 텍스트 셋팅
	int nCurDoneCount = 1;
	for (auto itDoneName : m_dcAddressMap.m_vecDoneName)
	{
		if (nCurDoneCount > m_dcAddressMap.m_nDoneCount)
			break;

		m_spreadConfig.SetText(nCurDoneCount++, 0, itDoneName);
	}

	for (int nIdx = nCurDoneCount; nIdx <= m_dcAddressMap.m_nDoneCount; nIdx++)
	{
		m_spreadConfig.SetText(nIdx, 0, _T("NoName"));
		m_dcAddressMap.m_vecDoneName.emplace_back(_T("NoName"));
	}

	if (m_dcAddressMap.m_vecDoneName.size() > m_dcAddressMap.m_nDoneCount)
		m_dcAddressMap.m_vecDoneName.resize(m_dcAddressMap.m_nDoneCount);


	// Item Name 헤더 텍스트 셋팅
	int nCurItemCount = 1;
	for (auto itDoneName : m_dcAddressMap.m_vecItemName)
	{
		if (nCurItemCount > m_dcAddressMap.m_nMItemCount)
			break;

		m_spreadConfig.SetText(-999, nCurItemCount++, itDoneName);
	}

	for (int nIdx = nCurItemCount; nIdx <= m_dcAddressMap.m_nMItemCount; nIdx++)
	{
		m_spreadConfig.SetText(-999, nIdx, _T("NoName"));
		m_dcAddressMap.m_vecItemName.emplace_back(_T("NoName"));
	}

	if (m_dcAddressMap.m_vecItemName.size() > m_dcAddressMap.m_nMItemCount)
		m_dcAddressMap.m_vecItemName.resize(m_dcAddressMap.m_nMItemCount);

	m_spreadConfig.SetText(-999, m_dcAddressMap.m_nMItemCount + 1, _T("Job Done"));
	
	m_spreadConfig.SetReDraw(TRUE);
}


CString CPlcTestDlg::GetRobotNoString(int nNo)
{
	CString strRobotNo = _T("");

	if (nNo % 2 == 1)
		strRobotNo.Format(_T("L%d"), nNo / 2 + 1);
	else
		strRobotNo.Format(_T("R%d"), nNo / 2);

	return strRobotNo;
}


int CPlcTestDlg::GetRobotNo(CString strRobotNo)
{
	int nRobotNo = 0;

	if (strRobotNo.GetAt(0) == 'L')
	{
		nRobotNo = (_ttoi(strRobotNo.Right(1)) * 2) - 1;
	}
	else if (strRobotNo.GetAt(0) == 'R')
	{
		nRobotNo = _ttoi(strRobotNo.Right(1)) * 2;
	}

	return nRobotNo;
}


void CPlcTestDlg::SetPlcConfigUI(int nPlcType)
{
	switch (nPlcType)
	{
	case PLCTYPE_MELSEC:
		GetDlgItem(IDC_STATIC_SPC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CB_SPC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PC_STATION_NO)->SetWindowText(_T("PC Station No :"));
		GetDlgItem(IDC_STATIC_PLC_STATION_NO)->SetWindowText(_T("PLC Station No :"));
		GetDlgItem(IDC_EDIT_PC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_NETWORK_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_CB_PLC_CPU_TYPE)->EnableWindow(TRUE);
		break;

	case PLCTYPE_SIEMENS:
		GetDlgItem(IDC_STATIC_SPC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CB_SPC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PC_STATION_NO)->SetWindowText(_T("Rack No :"));
		GetDlgItem(IDC_STATIC_PLC_STATION_NO)->SetWindowText(_T("Slot No :"));
		GetDlgItem(IDC_EDIT_PC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_NETWORK_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_CB_PLC_CPU_TYPE)->EnableWindow(FALSE);
		break;

	case PLCTYPE_AB:
		GetDlgItem(IDC_STATIC_SPC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CB_SPC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PC_STATION_NO)->SetWindowText(_T("Conn Serial No :"));
		GetDlgItem(IDC_STATIC_PLC_STATION_NO)->SetWindowText(_T("Slot No :"));
		GetDlgItem(IDC_EDIT_PC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_NETWORK_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_CB_PLC_CPU_TYPE)->EnableWindow(FALSE);
		break;

	case PLCTYPE_EMBEDDED:
		GetDlgItem(IDC_STATIC_SPC_LIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SPC_LIST)->SetWindowText(_T("SPC :"));
		GetDlgItem(IDC_CB_SPC_LIST)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_PC_STATION_NO)->SetWindowText(_T("Port :"));
		GetDlgItem(IDC_EDIT_PC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_STATION_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PLC_NETWORK_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_CB_PLC_CPU_TYPE)->EnableWindow(FALSE);
		break;

	case PLCTYPE_LS:
		GetDlgItem(IDC_STATIC_SPC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CB_SPC_LIST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_PC_STATION_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PLC_STATION_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PLC_NETWORK_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_CB_PLC_CPU_TYPE)->EnableWindow(FALSE);
		break;

	default:
		break;
	}
}


BOOL CPlcTestDlg::SetMelsecRegConfig(CString strZoneName, CSPSPlcConfig plcInfo)
{
	CRegistry reg;
	CString strRegPath;

	reg.SetRootKey(HKEY_LOCAL_MACHINE);

	//strRegPath = _T("SOFTWARE\\MITSUBISHI\\SWnD5-ACT\\COMMUTL\\LogicalStNo_0002\\STECMOPEN");
	strRegPath.Format(_T("SOFTWARE\\MITSUBISHI\\SWnD5-ACT\\COMMUTL\\LogicalStNo_00%02d\\STECMOPEN"), plcInfo.m_nLocationNo);

	if (reg.SetKey(strRegPath, FALSE))
	{
		reg.WriteString(_T("HostAddress"), plcInfo.m_strPlcIpAddr[0]);
		reg.WriteDword(_T("SourceStationNumber"), plcInfo.m_nPcStationNo);		//	PC
		reg.WriteDword(_T("StationNumber"), plcInfo.m_nPlcStationNo);			//	PLC
		reg.WriteDword(_T("NetworkNumber"), plcInfo.m_nPlcNetworkNo);
		reg.WriteDword(_T("SourceNetworkNumber"), plcInfo.m_nPlcNetworkNo);
		reg.WriteDword(_T("CpuType"), plcInfo.m_nPlcCpuType);

		//strRegPath = _T("SOFTWARE\\MITSUBISHI\\SWnD5-ACT\\COMMUTL\\LogicalStNo_0002\\UTL");
		strRegPath.Format(_T("SOFTWARE\\MITSUBISHI\\SWnD5-ACT\\COMMUTL\\LogicalStNo_00%02d\\UTL"), plcInfo.m_nLocationNo);

		if (reg.SetKey(strRegPath, FALSE))
		{
			if (plcInfo.m_nPlcCpuType == PLC_CPU_TYPE_12H)
			{
				reg.WriteString(_T("Comment"), strZoneName);
				reg.WriteDword(_T("UnitName"), plcInfo.m_nPlcCpuType);
				reg.WriteDword(_T("ConnectCpuSType"), 4099);
				reg.WriteDword(_T("CpuSType"), 4099);
			}
			else
			{
				reg.WriteString(_T("Comment"), strZoneName);
				reg.WriteDword(_T("UnitName"), plcInfo.m_nPlcCpuType);
				reg.WriteDword(_T("ConnectCpuSType"), 45061);
				reg.WriteDword(_T("CpuSType"), 45061);
			}
		}
	}

	return TRUE;
}


void CPlcTestDlg::ConvertStringNameToBuf(const CString & strName, vector<CString>& vecItemName)
{
	CString strParsedName;
	int nIndex = 0;

	vecItemName.clear();

	while (AfxExtractSubString(strParsedName, strName, nIndex, ','))
	{
		if (!strParsedName.IsEmpty())
		{
			vecItemName.push_back(strParsedName);
		}
		nIndex++;
	}
}


void CPlcTestDlg::ConvertBufToStringName(vector<CString>& vecItemName, CString & strName)
{
	int nVectorSize = 0;

	nVectorSize = vecItemName.size();

	for (int i = 0; i < nVectorSize; i++)
	{
		strName += vecItemName[i];
		strName += _T(",");
	}
}


void CPlcTestDlg::OnBnClickedBtnPlcConnect()
{
	UpdateData();

	CSPSPlcConfig plcInfo;
	plcInfo.m_nPlcType = m_cbPlcType.GetItemData(m_cbPlcType.GetCurSel());
	
	if (plcInfo.m_nPlcType == PLCTYPE_EMBEDDED)
	{
		m_editPlcIpAddress.GetWindowTextW(m_strTempPlcIP[m_cbSpcList.GetCurSel()]);
		for (int nIdx = 0; nIdx < MAX_PLC_IP_COUNT; nIdx++)
			plcInfo.m_strPlcIpAddr[nIdx] = m_strTempPlcIP[nIdx];
	}
	else
	{
		m_editPlcIpAddress.GetWindowTextW(m_strTempPlcIP[0]);
		plcInfo.m_strPlcIpAddr[0] = m_strTempPlcIP[0];
	}

	if (plcInfo.m_nPlcType == PLCTYPE_LS)
		plcInfo.m_nPcStationNo = 0;
	else
		plcInfo.m_nPcStationNo = m_nPcStationNo;

	if (plcInfo.m_nPlcType == PLCTYPE_LS || plcInfo.m_nPlcType == PLCTYPE_EMBEDDED)
		plcInfo.m_nPlcStationNo = 0;
	else
		plcInfo.m_nPlcStationNo = m_nPlcStationNo;

	if (plcInfo.m_nPlcType == PLCTYPE_MELSEC)
	{
		plcInfo.m_nPlcNetworkNo = m_nPlcNetworkNo;
		plcInfo.m_nPlcCpuType = m_cbPlcCpuType.GetCurSel() == 1 ? PLC_CPU_TYPE_13UDH : PLC_CPU_TYPE_12H;
		plcInfo.m_nLocationNo = COLLECTOR_MESECL_LOGICAL_STATION_ID;
	}
	else
	{
		plcInfo.m_nPlcNetworkNo = 0;
		plcInfo.m_nPlcCpuType = 0;
	}

	plcInfo.m_nLocationNo = 4;
	if (plcInfo.m_nPlcType == PLCTYPE_MELSEC)
		SetMelsecRegConfig(_T("Test PLC"), plcInfo);


	if (m_plcHandler.IsConnect() == FALSE)
	{
		if (m_plcHandler.Initialize(&m_SystemLog, plcInfo, m_dcAddressMap) == TRUE)
		{
			if (m_plcHandler.ConnectPlc() == TRUE)
			{
				SetConnectionStatus(true);
			}
			else
			{
				AfxMessageBox(_T("PLC connect failed! "), MB_ICONSTOP);
				SetConnectionStatus(false);
			}
		}
		else
		{
			AfxMessageBox(_T("PLC connect failed! "), MB_ICONSTOP);
			SetConnectionStatus(false);
		}
	}
	else
	{
		m_plcHandler.DisconnectPlc();
		m_plcHandler.DeInitialize();

		SetConnectionStatus(false);
	}
}


void CPlcTestDlg::AdvanceFpspreadCofig(BOOL AdvanceNext)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CPlcTestDlg::OnCbnSelchangeCbPlcType()
{
	int nPlcType = m_cbPlcType.GetItemData(m_cbPlcType.GetCurSel());
	SetPlcConfigUI(nPlcType);

	if (nPlcType == PLCTYPE_EMBEDDED)
		m_editPlcIpAddress.SetWindowTextW(m_strTempPlcIP[m_cbSpcList.GetCurSel()]);
	else
		m_editPlcIpAddress.SetWindowTextW(m_strTempPlcIP[0]);
}


void CPlcTestDlg::OnCbnSelchangeCbPlcCpuType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CPlcTestDlg::OnCbnSelchangeCbSpcList()
{
	int nCurSel = m_cbSpcList.GetCurSel();
	if (nCurSel > MAX_PLC_IP_COUNT)
		return;

	m_editPlcIpAddress.SetWindowTextW(m_strTempPlcIP[nCurSel]);
}


void CPlcTestDlg::OnCbnDropdownCbSpcList()
{
	int nCurSel = m_cbSpcList.GetCurSel();
	if (nCurSel > MAX_PLC_IP_COUNT)
		return;

	m_editPlcIpAddress.GetWindowTextW(m_strTempPlcIP[nCurSel]);
}


void CPlcTestDlg::OnBnClickedBtnReadHistorianData()
{
	if (m_plcHandler.IsConnect() == FALSE)
		return;

	m_mapHistorianData.clear();
	if (m_plcHandler.ReadHistorianData(m_mapHistorianData) == TRUE)
	{
		DispHistorianData();
	}
	else
	{
		AfxMessageBox(_T("Read historian data failed!"), MB_ICONSTOP);
	}
}



void CPlcTestDlg::OnBnClickedBtnWriteHistorainItemData()
{
	if (m_plcHandler.IsConnect() == FALSE)
		return;

	vector<SHORT> vecData;
	CString strTemp;
	for (int nCol = 1; nCol <= m_dcAddressMap.m_nDoneCount; nCol++)
	{
		m_spreadConfig.SetCol(nCol);

		for (int nRow = 1; nRow <= m_dcAddressMap.m_nMItemCount; nRow++)
		{
			m_spreadConfig.SetRow(nRow);
			strTemp = m_spreadConfig.GetText();

			vecData.emplace_back(_ttoi(strTemp));
		}
	}

	if (m_plcHandler.WriteHistorianItemData(vecData) == FALSE)
	{
		AfxMessageBox(_T("Write historian item data failed!"), MB_ICONSTOP);
	}
}


void CPlcTestDlg::OnBnClickedBtnWriteHistorianDoneData()
{
	if (m_plcHandler.IsConnect() == FALSE)
		return;

	vector<SHORT> vecData;
	CString strTemp;
	for (int nCol = 1; nCol <= m_dcAddressMap.m_nDoneCount; nCol++)
	{
		m_spreadConfig.SetCol(nCol);
		m_spreadConfig.SetRow(m_dcAddressMap.m_nMItemCount + 1);
		strTemp = m_spreadConfig.GetText();

		vecData.emplace_back(_ttoi(strTemp));
	}

	if (m_plcHandler.WriteHistorianDoneData(vecData) == FALSE)
	{
		AfxMessageBox(_T("Write historian done data failed!"), MB_ICONSTOP);
	}
}


void CPlcTestDlg::DispHistorianData()
{
	CString strData;
	for (auto itMap : m_mapHistorianData)
	{
		int nCol = itMap.first + 1;
		int nRow = 1;
		for (auto itData : itMap.second)
		{
			strData.Format(_T("%d"), itData);
			m_spreadConfig.SetText(nCol, nRow++, strData);
		}

		nCol++;
	}
}


void CPlcTestDlg::SetConnectionStatus(bool bConnected)
{
	if (bConnected)
	{
		GetDlgItem(IDC_CB_PLC_TYPE)->EnableWindow(FALSE);
		GetDlgItem(IDC_IPADDR_PLC_IP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PC_STATION_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PLC_STATION_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PLC_NETWORK_NO)->EnableWindow(FALSE);
		GetDlgItem(IDC_CB_PLC_CPU_TYPE)->EnableWindow(FALSE);

		GetDlgItem(IDC_BTN_PLC_CONNECT)->SetWindowTextW(_T("Disconnect"));
	}
	else
	{
		GetDlgItem(IDC_CB_PLC_TYPE)->EnableWindow(TRUE);
		GetDlgItem(IDC_IPADDR_PLC_IP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_STATION_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_PLC_NETWORK_NO)->EnableWindow(TRUE);
		GetDlgItem(IDC_CB_PLC_CPU_TYPE)->EnableWindow(TRUE);

		GetDlgItem(IDC_BTN_PLC_CONNECT)->SetWindowTextW(_T("Connect"));
	}
}


void CPlcTestDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_mapHistorianData.clear();

	if (m_plcHandler.IsConnect())
	{
		m_plcHandler.DisconnectPlc();
	}
	m_plcHandler.DeInitialize();

	m_SystemLog.Deinitialize();
}
