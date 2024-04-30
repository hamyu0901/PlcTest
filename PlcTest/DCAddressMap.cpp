#include "pch.h"
#include "DCAddressMap.h"


CDCAddressMap::CDCAddressMap(void)
{
	m_uVinno = 0;
}


CDCAddressMap::~CDCAddressMap(void)
{
	m_vecItemName.clear();
	m_vecDoneName.clear();
}


CDCAddressMap::CDCAddressMap(const CDCAddressMap & source)
{
	if (this == (&source))
	{
		return;
	}

	(*this) = source;
}


CDCAddressMap & CDCAddressMap::operator = (const CDCAddressMap & source)
{
	if (this == (&source))
	{
		return (*this);
	}

	m_strDevice = source.m_strDevice;
	m_strUsable = source.m_strUsable;
	m_nMStartAddr = source.m_nMStartAddr;
	m_nMEndAddr = source.m_nMEndAddr;
	m_nMItemCount = source.m_nMItemCount;
	m_nDStartAddr = source.m_nDStartAddr;
	m_nDEndAddr = source.m_nDEndAddr;
	m_nDoneCount = source.m_nDoneCount;
	m_uVinno = source.m_uVinno;

	m_bUseVinno = source.m_bUseVinno;
	m_uVinnoIdnex = source.m_uVinnoIdnex;
	m_uVinnoCount = source.m_uVinnoCount;

	m_vecItemName.clear();
	m_vecItemName.resize(source.m_vecItemName.size());
	copy(source.m_vecItemName.begin(), source.m_vecItemName.end(), m_vecItemName.begin());

	m_vecDoneName.clear();
	m_vecDoneName.resize(source.m_vecDoneName.size());
	copy(source.m_vecDoneName.begin(), source.m_vecDoneName.end(), m_vecDoneName.begin());

	return *this;
}
