#pragma once

class CDCAddressMap
{
public:
	CDCAddressMap(void);
	virtual ~CDCAddressMap(void);

	CDCAddressMap(const CDCAddressMap & source);
	CDCAddressMap & operator = (const CDCAddressMap & source);

	CString				m_strDevice;				///< Address Device Name
	CString				m_strUsable;
	int					m_nMStartAddr;				///< Monitoring Start Address
	int					m_nMEndAddr;				///< Monitoring End Address
	int					m_nMItemCount;				///< Monitoring Item Count
	vector<CString>		m_vecItemName;				///< Monitoring Items Name
	int					m_nDStartAddr;				///< Job Done Start Address
	int					m_nDEndAddr;				///< Job Done End Address
	int					m_nDoneCount;				///< Job Done OffSet
	UINT				m_uVinno;					///< VINNO
	vector<CString>		m_vecDoneName;

	BOOL				m_bUseVinno;				//< PLC용 Vinno 사용
	UINT				m_uVinnoIdnex;				//< PLC용 Vinno Index
	UINT				m_uVinnoCount;				//< PLC용 Vinno Count
};
