#include "XMLWrapper.h"
#include <assert.h>
#include <comutil.h>

/**
* CXMLWrapperDocument Constructor ...
*/
CXMLWrapperDocument::CXMLWrapperDocument()
{
	CoInitialize(NULL);
	Init();
	InitIndent();
}

/**
* CXMLWrapperDocument Constructor ...
*
* @param	ptr : IXMLDomDocument 스마트 포인터.
*/
CXMLWrapperDocument::CXMLWrapperDocument(CComPtr<IXMLDOMDocument>& ptr)
{
	m_document = ptr;

	m_szFileName[0] = _T('\0');
}

/**
* CXMLWrapperDocument Destructor ...
*
*/
CXMLWrapperDocument::~CXMLWrapperDocument()
{
	Term();
	CoUninitialize();
	InitIndent();
}

/**
* CXMLWrapperDocument 초기화. CLSID_DOMDocument COM 객체를 생성함.
*
*/
void CXMLWrapperDocument::Init()
{
	HRESULT hr = m_document.CoCreateInstance(CLSID_DOMDocument);
}

/**
* InitIndent
* Indent 파일 검사 후 초기화
*/
void CXMLWrapperDocument::InitIndent()
{
	CStdioFile fileStd;
	BOOL bRet = FALSE;
	TCHAR szBuff[MAX_PATH];
	TCHAR * pFilespec;
	CString strResult;
	CString strXslValue  = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\">\n	<xsl:output method=\"xml\" encoding=\"UTF-8\" indent=\"yes\"/>\n	<xsl:template match=\"*\">\n		<xsl:element name=\"{name(.)}\">\n			<xsl:apply-templates select=\"@*\"/>\n			<xsl:if test=\"child::* or child::text()\">\n				<xsl:apply-templates/>\n			</xsl:if>\n		</xsl:element>\n	</xsl:template>\n	<xsl:template match=\"@*\">\n		<xsl:attribute name=\"{name(.)}\"><xsl:value-of select=\".\"/></xsl:attribute>\n	</xsl:template>\n	<xsl:template match=\"processing-instruction()\">\n		<xsl:copy-of select=\".\"/>\n	</xsl:template>\n	<xsl:template match=\"comment()\">\n		<xsl:comment>\n			<xsl:value-of select=\".\"/>\n		</xsl:comment>\n	</xsl:template>\n</xsl:stylesheet>");

	m_strIndentPath = _T("");

	GetModuleFileName(NULL, szBuff, MAX_PATH);
	pFilespec = szBuff;
	::PathRemoveFileSpec(pFilespec);

	strResult = pFilespec;

	strResult += _T("\\");
	strResult += STR_INDENT_FILENAME;

	m_strIndentPath = strResult;

	bRet = fileStd.Open(strResult, CFile::modeReadWrite , NULL);

	if ( bRet )
	{
		return;
	}
	else
	{
		bRet = fileStd.Open(strResult, CFile::modeReadWrite | CFile::modeCreate , NULL);

		if ( bRet )
		{
			fileStd.WriteString(strXslValue);
		}
	}

	fileStd.Close();
}

/**
* Document 객채를 명시적으로 Release.(참조카운터감소).
*
*/
void CXMLWrapperDocument::Term()
{
	m_document.Release();
}

/**
* XML 문서를 Document객체로 Load.
*
* @param	pcszFileName : XML 문서의 Path
* @return	XML문서 Open 성공 여부
*/
bool CXMLWrapperDocument::Open(LPCTSTR pcszFileName)
{
	if( m_document.p == NULL )
	{
		Init();
	}

	assert( m_document.p != NULL );
	assert( pcszFileName != NULL );

	VARIANT	var;
	VARIANT_BOOL bSucessful;

	USES_CONVERSION;

	var.vt		= VT_BSTR;
	var.bstrVal = T2BSTR(pcszFileName);

	HRESULT hr = m_document->load(var, &bSucessful);
	if( FAILED(hr) || (hr == S_FALSE) )
	{
		::SysFreeString(var.bstrVal);
		return false;
	}

	_tcscpy_s(m_szFileName, _countof(m_szFileName), pcszFileName);
	::SysFreeString(var.bstrVal);

	return true;
}

/**
* iStream 객체를 통해 XML 문서를 Document객체로 Load.
*
* @param	pStream : IStream 객체
* @return	XML문서 Open 성공 여부
*/
bool CXMLWrapperDocument::Open(CComPtr<IStream> pStream)
{
	if( m_document.p == NULL )
	{
		Init();
	}

	assert( m_document.p != NULL );

	VARIANT	var;
	VARIANT_BOOL bSucessful;

	USES_CONVERSION;

	var.vt = VT_UNKNOWN;
	HRESULT hr = pStream.QueryInterface(&var.punkVal);
	assert(SUCCEEDED(hr));

	hr = m_document->load(var, &bSucessful);
	if( FAILED(hr) || (hr == S_FALSE) )
	{
		ATLTRACE(_T("CXMLWrapperDocument::Open: Failed\n"));
		return false;
	}

	m_szFileName[0] = _T('\0');

	return true;
}

/**
* XML 문서를 Document객체로 Load.
*
* @param	bstrName : XML 파일 경로(BSTR Type)
* @return	XML문서 Open 성공 여부
*/
bool CXMLWrapperDocument::Open(BSTR bstrName)
{
	if( m_document.p == NULL )
	{
		Init();
	}

	assert( m_document.p != NULL );

	VARIANT_BOOL bSucessful;

	HRESULT hr = m_document->loadXML(bstrName, &bSucessful);
	if( FAILED(hr) || (hr == S_FALSE) )
	{
		ATLTRACE(_T("CXMLWrapperDocument::Open: Failed\n"));
		return false;
	}

	m_szFileName[0] = _T('\0');

	return true;
}

/**
* XML Document객체의 사용을 완료 후 Document를 Close 하는 함수.
*
*/
void CXMLWrapperDocument::Close()
{
	if( m_document.p != NULL )
	{
		m_document.Release();
		m_szFileName[0] = _T('\0');
	}
}

/**
* XML Wrapper의 Document 객체를 교체하는 함수.
*
* @param	newDocument : 새로 교체될 document
*/
void CXMLWrapperDocument::Replace(CComPtr<IXMLDOMDocument>& newDocument)
{
	m_document.Release();

	m_document = newDocument;

	m_szFileName[0] = _T('\0');
}

/**
* XML 문서를 저장하는 함수.
*
* @param	pcszFileName : XML문서를 저장할 파일명
* @return	XML문서 저장 성공 여부
*/
bool CXMLWrapperDocument::Save(LPCTSTR pcszFileName)
{
	assert(pcszFileName != NULL);

	USES_CONVERSION;
	VARIANT var;

	var.vt		= VT_BSTR;
	var.bstrVal	= T2BSTR(pcszFileName);

	HRESULT hr = m_document->save(var);

	::SysFreeString(var.bstrVal);

	IndentDocument(pcszFileName);

	return (SUCCEEDED(hr) == TRUE);
}

/**
* XML Document에서 child 노드의 리스트를 가져온다.
*
* @return	child 노드 리스트의 반복자.
*/
CXMLWrapperIterator* CXMLWrapperDocument::GetChildren()
{
	IXMLDOMNodeList * pNodeList;
	HRESULT hr = m_document->get_childNodes(&pNodeList);
	assert(SUCCEEDED(hr));

	m_iterator.Init(pNodeList);
	pNodeList->Release();

	return &m_iterator;
}

/**
* XML Document에 프로퍼티를 설정.
*
* @param	pcszKey		: 프로퍼티 키
* @param	pcszValue	: 프로퍼티 값
*/
void CXMLWrapperDocument::SetProperty(LPCTSTR pcszKey, LPCTSTR pcszValue)
{
	USES_CONVERSION;

	BSTR bstrKey = T2BSTR(pcszKey);

	VARIANT var;
	HRESULT hr;

	var.vt		= VT_BSTR;
	var.bstrVal	= T2BSTR(pcszValue);

	CComPtr<IXMLDOMDocument2>	document;

	hr = m_document.QueryInterface(&document);

	hr = document->setProperty(bstrKey, var);
	assert(SUCCEEDED(hr));

	::SysFreeString(bstrKey);
	::SysFreeString(var.bstrVal);
}

/**
* XML Document에 프로퍼티를 가져옴.
*
* @param	pcszKey	: 가져올 프로퍼티 키
* @return	프로퍼티의 값
*/
CString CXMLWrapperDocument::GetProperty(LPCTSTR pcszKey)
{
	USES_CONVERSION;

	BSTR bstrKey = T2BSTR(pcszKey);

	VARIANT var;
	HRESULT hr;
	CString strProperty;
	CComPtr<IXMLDOMDocument2>	document;

	hr = m_document.QueryInterface(&document);

	hr = document->getProperty(bstrKey, &var);
	switch(var.vt)
	{
	case VT_EMPTY:
	case VT_NULL:
		strProperty.Empty();
		break;
	case VT_BSTR:
		strProperty = OLE2T(var.bstrVal);
		::SysFreeString(var.bstrVal);
		break;
	default:
		assert(FALSE);
		break;
	}

	::SysFreeString(bstrKey);
	return strProperty;
}

/**
* Element를 생성.
*
* @param	pcszTagName	: Element 테그명
* @return	생성된 Element 노드
*/
CXMLWrapperNode CXMLWrapperDocument::CreateElement(LPCTSTR pcszTagName)
{
	USES_CONVERSION;
	BSTR bstrTagName = T2BSTR(pcszTagName);

	IXMLDOMElement * pElement = NULL;
	HRESULT hr = m_document->createElement(bstrTagName, &pElement);
	assert(SUCCEEDED(hr));

	IXMLDOMNode * pNode = NULL;
	if( pElement )
	{
		hr = pElement->QueryInterface(__uuidof(IXMLDOMNode), reinterpret_cast<void**>(&pNode));
		assert(SUCCEEDED(hr));
	}

	CXMLWrapperNode createNode(pNode);

	if( pNode ) 	pNode->Release();
	if( pElement )	pElement->Release();

	::SysFreeString(bstrTagName);

	return createNode;
}

/**
* Text 노드를 생성.
*
* @param	pcszTextNode : 표시할 문자열
* @return	생성된 텍스트 노드
*/
CXMLWrapperNode CXMLWrapperDocument::CreateTextNode(LPCTSTR pcszTextNode)
{
	USES_CONVERSION;
	BSTR bstrTextNode = T2BSTR(pcszTextNode);

	IXMLDOMText * pText = NULL;
	HRESULT h = m_document->createTextNode(bstrTextNode, &pText);
	assert(SUCCEEDED(h));

	IXMLDOMNode* pNode = NULL;
	if( pText )
	{
		h = pText->QueryInterface(__uuidof(IXMLDOMNode), reinterpret_cast<void**>(&pNode));
		assert(SUCCEEDED(h));
	}

	CXMLWrapperNode createNode(pNode);
	if( pNode )	pNode->Release();
	if( pText )	pText->Release();

	::SysFreeString(bstrTextNode);

	return createNode;
}

/**
* Attribute 노드를 생성.
*
* @param	pcszAttrName : Attribute 이름
* @return	생성된 Attribute 노드
*/
CXMLWrapperNode CXMLWrapperDocument::CreateAttribute(LPCTSTR pcszAttrName)
{
	USES_CONVERSION;

	BSTR bstrAttrName = T2BSTR(pcszAttrName);

	IXMLDOMAttribute* pAttribute = NULL;
	HRESULT hr = m_document->createAttribute(bstrAttrName, &pAttribute);
	assert(SUCCEEDED(hr));

	IXMLDOMNode* pNode = NULL;
	if( pAttribute )
	{
		hr = pAttribute->QueryInterface(__uuidof(IXMLDOMNode), reinterpret_cast<void**>(&pNode));
		assert(SUCCEEDED(hr));
	}

	CXMLWrapperNode createNode(pNode);

	if( pNode )			pNode->Release();
	if( pAttribute )	pAttribute->Release();

	::SysFreeString(bstrAttrName);

	return createNode;
}

/**
* Attribute 노드를 생성.
*
* @param	node : 속성노드 Parent
* @param	pcszAttrName : Attribute 이름
* @param	lpszAttrValue : Attribute 값
* @return	성공 / 실패
*/
BOOL CXMLWrapperDocument::CreateAttribute(CXMLWrapperNode & node, LPCTSTR pcszAttrName, LPCTSTR lpszAttrValue)
{
	USES_CONVERSION;

	BSTR bstrAttrName;
	VARIANT varNodeValue;
	IXMLDOMAttribute * pAttribute = NULL;

	bstrAttrName			= T2BSTR(pcszAttrName);
	varNodeValue.bstrVal	= T2BSTR(lpszAttrValue);
	varNodeValue.vt			= VT_BSTR;

	HRESULT hr = m_document->createAttribute(bstrAttrName, &pAttribute);
	if (FAILED(hr) )
	{
		return FALSE;
	}

	pAttribute->put_value(varNodeValue);
	if (FAILED(hr) )
	{
		return FALSE;
	}

	IXMLDOMNamedNodeMap * pNamedNodeMap = NULL;
	hr = node.m_pNode->get_attributes(&pNamedNodeMap);
	if (FAILED(hr) )
	{
		return FALSE;
	}

	IXMLDOMNode* pNamedItem = NULL;
	hr = pNamedNodeMap->setNamedItem(pAttribute, &pNamedItem);
	if (FAILED(hr) )
	{
		return FALSE;
	}

	if( pAttribute )	pAttribute->Release();
	if( pNamedItem )	pNamedItem->Release();
	if( pNamedNodeMap ) pNamedNodeMap->Release();

	::SysFreeString(bstrAttrName);
	::SysFreeString(varNodeValue.bstrVal);

	return TRUE;
}

/**
* Attribute 노드를 생성.
*
* @param	node : 속성노드 Parent
* @param	pcszAttrName : Attribute 이름
* @param	lpszAttrValue : Attribute 값
* @return	성공 / 실패
*/
BOOL CXMLWrapperDocument::CreateAttribute(IXMLDOMNamedNodeMap * pNamedNodeMap, LPCTSTR pcszAttrName, LPCTSTR lpszAttrValue)
{
	USES_CONVERSION;

	BSTR bstrAttrName;
	VARIANT varNodeValue;

	CXMLWrapperNodeMap nodeMap;
	IXMLDOMAttribute * pAttribute = NULL;

	bstrAttrName			= T2BSTR(pcszAttrName);
	varNodeValue.bstrVal	= T2BSTR(lpszAttrValue);
	varNodeValue.vt			= VT_BSTR;

	HRESULT hr = m_document->createAttribute(bstrAttrName, &pAttribute);
	if (FAILED(hr) )
	{
		return FALSE;
	}

	pAttribute->put_value(varNodeValue);
	if (FAILED(hr) )
	{
		return FALSE;
	}

	IXMLDOMNode* pNamedItem = NULL;
	hr = pNamedNodeMap->setNamedItem(pAttribute, &pNamedItem);
	if (FAILED(hr) )
	{
		return FALSE;
	}

	if( pAttribute )	pAttribute->Release();
	if( pNamedItem )	pNamedItem->Release();

	::SysFreeString(bstrAttrName);
	::SysFreeString(varNodeValue.bstrVal);

	return TRUE;
}

/**
* Entity 노드를 생성.
*
* @param	pcszEntityName : Entity 이름
* @return	생성된 Entity 노드
*/
CXMLWrapperNode CXMLWrapperDocument::CreateEntityReference(LPCTSTR pcszEntityName)
{
	USES_CONVERSION;
	BSTR bstrEntityName = T2BSTR(pcszEntityName);

	IXMLDOMEntityReference* pEntityReference = NULL;;
	HRESULT hr = m_document->createEntityReference(bstrEntityName, &pEntityReference);
	assert(SUCCEEDED(hr));

	IXMLDOMNode* pNode = NULL;
	if( pEntityReference )
	{
		hr = pEntityReference->QueryInterface(__uuidof(IXMLDOMNode), reinterpret_cast<void**>(&pNode));
		assert(SUCCEEDED(hr));
	}

	CXMLWrapperNode createNode(pNode);

	if( pNode )				pNode->Release();
	if( pEntityReference )	pEntityReference->Release();

	::SysFreeString(bstrEntityName);

	return createNode;
}

/**
* Fragment 노드를 생성.
*
* @return	생성된 Fragment 노드
*/
CXMLWrapperNode CXMLWrapperDocument::CreateDocumentFragment()
{
	IXMLDOMDocumentFragment* pDocumentFragment;
	HRESULT hr = m_document->createDocumentFragment(&pDocumentFragment);
	assert(SUCCEEDED(hr));

	IXMLDOMNode* pNode = NULL;

	if( pDocumentFragment )
	{
		hr = pDocumentFragment->QueryInterface(__uuidof(IXMLDOMNode), reinterpret_cast<void**>(&pNode));
		assert(SUCCEEDED(hr));
	}

	CXMLWrapperNode createNode(pNode);
	if( pNode )				pNode->Release();
	if( pDocumentFragment )	pDocumentFragment->Release();

	return createNode;
}

/**
* 지정된 노드에 Attribute를 입력함
*
* @param	node			: Attribute를 입력할 노드
* @param	pcszAttrName	: Attribute 이름
* @param	pcszAttrValue	: Attribute 값
* @return	입력 성공 여부
*/
BOOL CXMLWrapperDocument::InsertNodeAttribute(CXMLWrapperNode & node, LPCTSTR pcszAttrName, LPCTSTR pcszAttrValue)
{
	if( CreateAttribute(node, pcszAttrName, pcszAttrValue) == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}

/**
* XML 문서 Load, Save시 공백을 처리하는 방법을 결정
*
* @param	bPreserving : true 공백유지 / false 유효공백만 유지
*/
void CXMLWrapperDocument::PreServeWhiteSpace(bool bPreserving)
{
	m_document->put_preserveWhiteSpace(bPreserving ? VARIANT_TRUE : VARIANT_FALSE);
}

/**
* 최상위(ROOT) 노드를 가져옴
*
* @return	root 노드
*/
CXMLWrapperNode CXMLWrapperDocument::GetRootNode()
{
	IXMLDOMNode* pNode = NULL;
	HRESULT hr = m_document.QueryInterface(&pNode);
	assert(SUCCEEDED(hr));

	CXMLWrapperNode nodeRoot(pNode);
	pNode->Release();

	return nodeRoot;
}

CXMLWrapperNode* CXMLWrapperDocument::GetRootNodePtr()
{
	IXMLDOMNode* pNode = NULL;
	HRESULT hr = m_document.QueryInterface(&pNode);
	assert(SUCCEEDED(hr));

	CXMLWrapperNode* pNew = new CXMLWrapperNode(pNode);
	pNode->Release();

	//return nodeRoot;
	return pNew;
}

/**
* Document에 Element 입력.
*
* @param	pNode : 입력할 Element 노드
*/
void CXMLWrapperDocument::PutRefDocumentElement(CXMLWrapperNode * pNode)
{
	IXMLDOMElement* pElement = NULL;
	HRESULT hr = pNode->m_pNode.QueryInterface(&pElement);
	assert(SUCCEEDED(hr));

	hr = m_document->putref_documentElement(pElement);
	assert(SUCCEEDED(hr));

	pElement->Release();
}

/**
* Document에 Element 가져옴.
*
* @return	Element 노드
*/
CXMLWrapperNode CXMLWrapperDocument::GetDocumentElement()
{
	IXMLDOMElement* pElement;
	HRESULT hr = m_document->get_documentElement(&pElement);
	assert(SUCCEEDED(hr));

	IXMLDOMNode* pNode = NULL;
	if( pElement )
	{
		hr = pElement->QueryInterface(__uuidof(IXMLDOMNode), reinterpret_cast<void**>(&pNode));
		assert(SUCCEEDED(hr));
	}

	CXMLWrapperNode nodeDocumentElement(pNode);
	if( pNode )		pNode->Release();
	if( pElement )	pElement->Release();

	return nodeDocumentElement;
}

/**
* Document의 스마트 포인터 가져오기.
*
*/
CComPtr<IXMLDOMDocument> CXMLWrapperDocument::GetDocument()
{
	return m_document;
}

/**
* IndentDocument
* XSL이용 XML 파일 들여쓰기
* @param CString strInputXmlPath : XML 경로
* @return	성공 / 실패
*/
BOOL CXMLWrapperDocument::IndentDocument(CString strInputXmlPath)
{
	// 문서 객체, 결과객체선언
	CComPtr<IXMLDOMDocument> pXML = NULL, pXSL = NULL, pResult = NULL; // 원본파일, // 스타일시트파일, // 저장할결과파일
	HRESULT hr = S_OK;
	VARIANT vObject, var;
	VARIANT_BOOL bSucessful;

	USES_CONVERSION;

	var.vt		= VT_BSTR;
	var.bstrVal = T2BSTR(strInputXmlPath);

	// 원본 XML 파일불러오기
	pXML.CoCreateInstance(__uuidof(DOMDocument));
	pXML->put_async(VARIANT_FALSE);
	hr = pXML->load(var, &bSucessful);

	::SysFreeString(var.bstrVal);

	if ( hr == S_FALSE )
	{
		OutputDebugString(_T("Input File Load Error"));
		return FALSE;
	}

	var.vt		= VT_BSTR;
	var.bstrVal = T2BSTR(m_strIndentPath);

	// 스타일시트파일 불러오기
	pXSL.CoCreateInstance(__uuidof(DOMDocument));
	pXSL->put_async(VARIANT_FALSE);
	hr = pXSL->load(var, &bSucessful); // XSL 파일 기술

	::SysFreeString(var.bstrVal);

	if ( hr == S_FALSE )
	{
		OutputDebugString(_T("XSL File Load Error"));
		return FALSE;
	}

	// 결과파일객체초기화
	pResult.CoCreateInstance(__uuidof(DOMDocument));

	// 변환 관련변수생성및초기화
	VariantInit(&vObject);
	vObject.vt = VT_DISPATCH;
	vObject.pdispVal = pResult;

	// 변환 수행
	hr = pXML->transformNodeToObject(pXSL, vObject);
	if ( hr == E_FAIL )
	{
		OutputDebugString(_T("Transformation Error"));
		return FALSE;
	}

	// 변환된파일 저장하기
	hr = pResult->save((_variant_t)strInputXmlPath);
	if( hr == E_FAIL )
	{
		OutputDebugString(_T("Output File Save Error"));
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//	CXMLWrapperIteratorNode
////////////////////////////////////////////////////////////////////////////////
/**
* CXMLWrapperIteratorNodes Constructor
*
*/
CXMLWrapperIteratorNodes::CXMLWrapperIteratorNodes()
{
	static bool first = true;
	if( first == true )
	{
		first	= false;
		m_pNode	= new CXMLWrapperNode;
		first	= true;
	}
}

/**
* CXMLWrapperIteratorNodes Destructor
*
*/
CXMLWrapperIteratorNodes::~CXMLWrapperIteratorNodes()
{
	static bool first = true;
	if( first == true )
	{
		first = false;
		delete m_pNode;
		first = true;
	}
}

/**
* 입력된 노드 리스트로 초기화
*
* @param	pNodeList : 입력할 노드 리스트
*/
void CXMLWrapperIteratorNodes::Init(IXMLDOMNodeList * pNodeList)
{
	m_pNodeList = pNodeList;
}

/**
* 노드 리스트로 시작 위치로 반복자를 이동.
*
*/
void CXMLWrapperIteratorNodes::Begin()
{
	if( m_pNodeList.p == NULL )
	{
		return;
	}

	m_pNodeList->reset();

	Next();
}

/**
* 노드 리스트를 다음 반복자 위치로 이동.
*
*/
void CXMLWrapperIteratorNodes::Next()
{
	if( m_pNode->IsNull() == false )
	{
		m_pNode->Term();
	}

	CComPtr<IXMLDOMNode> pNode;
	m_pNodeList->nextNode(&pNode);
	m_pNode->Init(pNode);
}

/**
* 현재 지정된 반복자가 유효한지 확인.
*
* @return	반복자의 유효성
*/
bool CXMLWrapperIteratorNodes::IsDone()
{
	if( m_pNodeList.p == NULL )
	{
		return true;
	}

	return m_pNode->IsNull();
}

/**
* 현재 지정된 반복자의 노드를 가져옴.
*
* @return	현재 지정된 반복자의 노드.
*/
CXMLWrapperNode CXMLWrapperIteratorNodes::CurrentItem()
{
	return (*m_pNode);
}

/**
* 노드의 총 개수를 가져옴
*
* @return	노드의 개수
*/
LONG CXMLWrapperIteratorNodes::GetNodeCount()
{
	LONG lCount = 0;
	if( m_pNodeList.p == NULL )
	{
		return lCount;
	}

	m_pNodeList->get_length(&lCount);

	return lCount;
}

////////////////////////////////////////////////////////////////////////////////
//	CXMLWrapperNode
////////////////////////////////////////////////////////////////////////////////

/**
* CXMLWrapperNode Constructor..
*
*/
CXMLWrapperNode::CXMLWrapperNode()
{
	m_pNode.p = NULL;
}

/**
* CXMLWrapperNode Constructor..
*
*/
CXMLWrapperNode::CXMLWrapperNode(const CXMLWrapperNode& node)
{
	m_pNode = node.m_pNode;
}

/**
* CXMLWrapperNode Constructor..
*
*/
CXMLWrapperNode::CXMLWrapperNode(IXMLDOMNode* pNode)
{
	m_pNode = pNode;
}

/**
* CXMLWrapperNode operator =
*
*/
void CXMLWrapperNode::operator=(const CXMLWrapperNode& node)
{
	m_pNode = node.m_pNode;
}

/**
* CXMLWrapperNode Init
*
*/
void CXMLWrapperNode::Init(IXMLDOMNode* pNode)
{
	m_pNode = pNode;
}

/**
* CXMLWrapperNode Term
*
*/
void CXMLWrapperNode::Term()
{
	m_pNode.Release();
}

/**
* 노드의 유효성을 확인.
*
* @return	노드의 유효성.
*/
bool CXMLWrapperNode::IsNull()
{
	return (m_pNode.p == NULL);
}

/**
* 노드의 데이터 타입을 반환
*
* @return	노드의 데이터 타입
*/
CString CXMLWrapperNode::GetDataType()
{
	VARIANT var;

	var.vt = VT_EMPTY;
	m_pNode->get_dataType(&var);

	CString strValue;
	if( (var.vt == VT_EMPTY) || (var.vt == VT_NULL) )
	{
		strValue.Empty();
	}
	else
	{
		USES_CONVERSION;
		strValue = OLE2CT(var.bstrVal);
	}

	::SysFreeString(var.bstrVal);

	return strValue;
}

/**
* 노드의 데이터 타입을 입력
*
* @param	pcszType : 입력할 데이터 타입
*/
void CXMLWrapperNode::PutDataType(LPCTSTR pcszType)
{
	assert(pcszType != NULL);

	USES_CONVERSION;
	BSTR bstrType;

	bstrType = T2BSTR(pcszType);

	m_pNode->put_dataType(bstrType);

	::SysFreeString(bstrType);
}

/**
* 노드의 테그이름을 반환
*
* @return	노드의 테그 이름
*/
CString CXMLWrapperNode::GetNodeName()
{
	USES_CONVERSION;

	BSTR bstrNodeName;
	CString strNodeName;

	HRESULT hr = m_pNode->get_nodeName(&bstrNodeName);
	assert(SUCCEEDED(hr));

	strNodeName = OLE2T(bstrNodeName);

	::SysFreeString(bstrNodeName);

	return strNodeName;
}

/**
* 현재 노드의 child노드의 반복자를 반환.
*
* @return	child노드의 반복자
*/
CXMLWrapperIterator* CXMLWrapperNode::GetChildren()
{
	HRESULT hr;
	IXMLDOMNodeList* pNodeList;

	switch(GetNodeType())
	{
	case NODE_TEXT:
		pNodeList = NULL;
		break;
	default:
		hr = m_pNode->get_childNodes(&pNodeList);
		assert(SUCCEEDED(hr));
		break;
	}

	m_iteratorNodes.Init(pNodeList);
	if( pNodeList )
	{
		pNodeList->Release();
	}

	return static_cast<CXMLWrapperIterator*>(&m_iteratorNodes);
}

/**
* 인덱스에 해당하는 child노드를 반환
*
* @param	lIndex : 찾을 노드의 인덱스
* @return	child노드
*/
CXMLWrapperNode CXMLWrapperNode::GetChild(long lIndex)
{
	CXMLWrapperIterator * pIt = GetChildren();
	if( pIt == NULL )
	{
		return NULL;
	}

	for( pIt->Begin(); ((lIndex > 0) && pIt->IsDone() == false); pIt->Next(), --lIndex )
	{
		;
	}

	if( pIt->IsDone() == true )
	{
		return CXMLWrapperNode();
	}

	return pIt->CurrentItem();
}

long CXMLWrapperNode::GetChildCount()
{
	long lCount = 0;
	CXMLWrapperIterator * pIt = GetChildren();

	if( pIt == NULL )
	{
		return lCount;
	}

	for( pIt->Begin(); (pIt->IsDone() == false); pIt->Next() )
	{
		lCount ++;
	}
	return lCount;
}

/**
* 노드의 타입 반환
*
* @return	노드타입
*/
DOMNodeType CXMLWrapperNode::GetNodeType()
{
	DOMNodeType domNodeType;

	HRESULT hr = m_pNode->get_nodeType(&domNodeType);
	assert(SUCCEEDED(hr));

	return domNodeType;
}

/**
* 노드의 값을 반환
*
* @param	Int2Type<VT_BSTR> : BSTR 타입을 컴파일 시간에 지정
* @return	노드 값.
*/
CString CXMLWrapperNode::GetNodeTypedValue(Int2Type<VT_BSTR>)
{
	VARIANT var;
	HRESULT hr = m_pNode->get_nodeTypedValue(&var);
	assert(SUCCEEDED(hr));

	USES_CONVERSION;
	CString strNodeTypeValue;

	switch(var.vt)
	{
	case VT_BSTR:
		strNodeTypeValue = OLE2T(var.bstrVal);
		::SysFreeString(var.bstrVal);
		break;
	case VT_EMPTY:
	case VT_NULL:
		strNodeTypeValue.Empty();
		break;
	default:
		ATLTRACE(_T("get_nodeTypedValue:Invalid Data Type\n"));
		assert(FALSE);
		break;
	}

	return strNodeTypeValue;
}

/**
* 노드의 값을 반환
*
* @param	Int2Type<VT_I4> : long 타입을 컴파일 시간에 지정
* @return	노드 값.
*/
long CXMLWrapperNode::GetNodeTypedValue(Int2Type<VT_I4>)
{
	VARIANT var;
	HRESULT hr = m_pNode->get_nodeTypedValue(&var);
	assert(SUCCEEDED(hr));

	switch(var.vt)
	{
	case VT_I1:
	case VT_I2:
	case VT_I4:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
		return var.lVal;
	default:
		ATLTRACE(_T("get_nodeTypedValue:Invalid Data Type\n"));
		assert(FALSE);
		break;
	}

	assert(FALSE);
	return -1;
}

/**
* 노드의 값을 반환
*
* @param	Int2Type<VT_BOOL> : bool 타입을 컴파일 시간에 지정
* @return	노드 값.
*/
bool CXMLWrapperNode::GetNodeTypedValue(Int2Type<VT_BOOL>)
{
	VARIANT var;
	HRESULT hr = m_pNode->get_nodeTypedValue(&var);
	assert(SUCCEEDED(hr));

	switch(var.vt)
	{
	case VT_BOOL:
		return (var.boolVal == VARIANT_TRUE);

	case VT_EMPTY:
	case VT_NULL:
		return false;

	default:
		ATLTRACE(_T("get_nodeTypedValue:Invalid Data Type\n"));
		assert(FALSE);
		break;
	}

	assert(FALSE);
	return false;
}

/**
* 노드의 값을 반환
*
* @param	Int2Type<VT_R8> : double 타입을 컴파일 시간에 지정
* @return	노드 값.
*/
double CXMLWrapperNode::GetNodeTypedValue(Int2Type<VT_R8>)
{
	VARIANT var;
	HRESULT hr = m_pNode->get_nodeTypedValue(&var);
	assert(SUCCEEDED(hr));

	switch(var.vt)
	{
	case VT_R4:
	case VT_R8:
		return var.dblVal;

	case VT_EMPTY:
	case VT_NULL:
		return 0.0f;

	default:
		ATLTRACE(_T("get_nodeTypedValue:Invalid Data Type\n"));
		assert(FALSE);
		break;
	}

	assert(FALSE);
	return 0.0f;
}

/**
* 노드의 값을 입력
*
* @param	pcszValue : 입력 문자열
*/
void CXMLWrapperNode::PutNodeTypedValue(LPCTSTR pcszValue)
{
	USES_CONVERSION;
	VARIANT var;

	var.vt		= VT_BSTR;
	var.bstrVal	= T2BSTR(pcszValue);

	HRESULT hr = m_pNode->put_nodeTypedValue(var);
	assert(SUCCEEDED(hr));

	::SysFreeString(var.bstrVal);
}

/**
* 노드의 값을 입력
*
* @param	lValue : long 값
*/
void CXMLWrapperNode::PutNodeTypedValue(long lValue)
{
	USES_CONVERSION;
	VARIANT var;
	var.vt		= VT_I4;
	var.lVal	= lValue;

	HRESULT hr = m_pNode->put_nodeTypedValue(var);
	assert(SUCCEEDED(hr));
}

/**
* 노드의 값을 입력
*
* @param	iValue : int 값
*/
void CXMLWrapperNode::PutNodeTypedValue(int iValue)
{
	USES_CONVERSION;
	VARIANT var;
	var.vt		= VT_I4;
	var.lVal	= iValue;

	HRESULT hr = m_pNode->put_nodeTypedValue(var);
	assert(SUCCEEDED(hr));
}

/**
* 노드의 값을 입력
*
* @param	uValue : unsigned int 값
*/
void CXMLWrapperNode::PutNodeTypedValue(UINT uValue)
{
	USES_CONVERSION;
	VARIANT var;
	var.vt		= VT_I4;
	var.lVal	= uValue;

	HRESULT hr = m_pNode->put_nodeTypedValue(var);
	assert(SUCCEEDED(hr));
}

/**
* 노드의 값을 입력
*
* @param	bValue : bool 값
*/
void CXMLWrapperNode::PutNodeTypedValue(bool bValue)
{
	USES_CONVERSION;
	VARIANT var;
	var.vt		= VT_BOOL;
	var.boolVal	= (bValue) ? VARIANT_TRUE : VARIANT_FALSE;

	HRESULT hr = m_pNode->put_nodeTypedValue(var);
	assert(SUCCEEDED(hr));
}

/**
* 노드의 값을 입력
*
* @param	dValue : double 값
*/
void CXMLWrapperNode::PutNodeTypedValue(double dValue)
{
	USES_CONVERSION;
	VARIANT var;
	var.vt		= VT_R8;
	var.dblVal	= dValue;

	HRESULT hr = m_pNode->put_nodeTypedValue(var);
	assert(SUCCEEDED(hr));
}

/**
* 노드의 값을 반환
*
* @return	노드의 값
*/
CString CXMLWrapperNode::GetNodeValue()
{
	VARIANT var;
	HRESULT hr = m_pNode->get_nodeTypedValue(&var);
	assert(SUCCEEDED(hr));

	USES_CONVERSION;
	CString strNodeValue;
	switch(var.vt)
	{
	case VT_BSTR:
		strNodeValue = OLE2T(var.bstrVal);
		::SysFreeString(var.bstrVal);
		break;
	case VT_EMPTY:
	case VT_NULL:
		strNodeValue.Empty();
		break;
	default:
		ATLTRACE(_T("get_nodeTypedValue:Invalid Data Type\n"));
		assert(FALSE);
		break;
	}

	return strNodeValue;
}

/**
* 노드의 값을 반환
*
* @param	pcszAttrName : Attribute 이름
* @param	strAttrValue : [out] Attribute 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::GetNodeAttrValue(LPCTSTR pcszAttrName, CString & strAttrValue)
{
	CXMLWrapperNodeMap nodeMap;

	IXMLDOMNamedNodeMap * pNamedNodeMap = NULL;
	HRESULT hr = m_pNode->get_attributes(&pNamedNodeMap);
	assert(SUCCEEDED(hr));

	nodeMap.Init(pNamedNodeMap);

	if( pNamedNodeMap )
	{
		pNamedNodeMap->Release();
	}

	if( nodeMap.IsNull() == false )
	{
		return nodeMap.ValueOfItem(pcszAttrName, strAttrValue);
	}

	return FALSE;
}

/**
* 노드의 값을 변경
*
* @param	pcszAttrName : Attribute 이름
* @param	strAttrValue : Attribute 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::SetNodeAttrValue(LPCTSTR pcszAttrName, LPCTSTR pcszAttrValue)
{
	CString strNodeName;
	CXMLWrapperNode attributeNode;
	CXMLWrapperIterator * pIter;
	CXMLWrapperNodeMap nodeMap;

	nodeMap = GetAttributes();
	if( nodeMap.IsNull() == false )
	{
		pIter = nodeMap.GetIterator();
		for( pIter->Begin(); pIter->IsDone() == false; pIter->Next() )
		{
			attributeNode = pIter->CurrentItem();
			if( attributeNode.IsNull() == false )
			{
				strNodeName = attributeNode.GetNodeName();

				if( strNodeName == pcszAttrName )
				{
					attributeNode.PutNodeValue(pcszAttrValue);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

/**
* 노드에 문자열 값을 입력
*
* @param	pcszValue : 입력 값
*/
void CXMLWrapperNode::PutNodeValue(LPCTSTR pcszValue)
{
	USES_CONVERSION;

	VARIANT var;
	var.vt		= VT_BSTR;
	var.bstrVal	= T2BSTR(pcszValue);

	HRESULT hr = m_pNode->put_nodeTypedValue(var);
	assert(SUCCEEDED(hr));

	::SysFreeString(var.bstrVal);
}

/**
* 현재 노드에 child 노드를 추가
*
* @param	pNode	  : 추가할 child 노드
* @return	성공 여부
*/
bool CXMLWrapperNode::AppendChild(CXMLWrapperNode * pNode)
{
	IXMLDOMNode * pNewNode = NULL;

	HRESULT hr = m_pNode->appendChild(pNode->m_pNode, &pNewNode);
	if( FAILED(hr) )
	{
		IXMLError* pError = NULL;
		hr = m_pNode->QueryInterface(IID_IXMLError, reinterpret_cast<void**>(&pError));
		assert(SUCCEEDED(hr));

		XML_ERROR xmlErr;

		if( pError )		pError->GetErrorInfo(&xmlErr);

		_ASSERTE(FALSE);
	}

	if( pNewNode )
	{
		pNewNode->Release();
	}

	return true;
}

/**
* 현재 노드가 child 노드를 가지고 있는지 확인
*
* @return	child 노드 유무
*/
bool CXMLWrapperNode::HasChildNodes()
{
	VARIANT_BOOL varBool;

	HRESULT hr = m_pNode->hasChildNodes(&varBool);
	assert(SUCCEEDED(hr));

	return (varBool == VARIANT_TRUE);
}

/**
* 현재 노드에서 지정한 child 노드를 삭제
*
* @param	pNode	  : 삭제할 child 노드
*/
void CXMLWrapperNode::RemoveChild(CXMLWrapperNode * pNode)
{
	IXMLDOMNode * pOldNodeCopy = NULL;

	HRESULT h = m_pNode->removeChild(pNode->m_pNode, &pOldNodeCopy);
	assert(SUCCEEDED(h));

	if( pOldNodeCopy )
	{
		pOldNodeCopy->Release();
	}
}

/**
* 현재 노드에서 지정한 child를 교체
*
* @param	pNewNode	  : 새로 변경할 child 노드
* @param	pOldNode	  : 변경 대상 child 노드
* @return	교채 성공 여부
*/
bool CXMLWrapperNode::ReplaceChild(CXMLWrapperNode * pNewNode, CXMLWrapperNode * pOldNode)
{
	IXMLDOMNode * pOldNodeCopy = NULL;
	HRESULT hr = m_pNode->replaceChild(pNewNode->m_pNode, pOldNode->m_pNode, &pOldNodeCopy);
	assert(SUCCEEDED(hr));

	if( pOldNodeCopy )
	{
		pOldNodeCopy->Release();
	}

	return true;
}

/**
* 파라미터에 해당하는 노드 리스트를 반환
*
* @param	pcszExpression	  : 찾을 노드의 리스트
* @return	노드 리스트 반복자
*/
CXMLWrapperIterator* CXMLWrapperNode::SelectNodes(LPCTSTR pcszExpression)
{
	IXMLDOMNodeList * pNodeList = NULL;
	BSTR bstrExpression;

	USES_CONVERSION;
	bstrExpression = T2BSTR(pcszExpression);

	HRESULT hr = m_pNode->selectNodes(bstrExpression, &pNodeList);
	assert(SUCCEEDED(hr));

	m_iteratorNodes.Init(pNodeList);
	::SysFreeString(bstrExpression);
	if( pNodeList )
	{
		pNodeList->Release();
	}

	return &m_iteratorNodes;
}

/**
* 파라미터에 해당하는 노드를 반환
*
* @param	pcszExpression	  : 찾을 노드
* @return	노드
*/
CXMLWrapperNode CXMLWrapperNode::SelectSingleNode(LPCTSTR pcszExpression)
{
	BSTR bstrExpression;
	USES_CONVERSION;

	bstrExpression = T2BSTR(pcszExpression);

	IXMLDOMNode * pNode;
	HRESULT hr = m_pNode->selectSingleNode(bstrExpression, &pNode);
	assert(SUCCEEDED(hr));
	::SysFreeString(bstrExpression);

	CXMLWrapperNode node;
	node.Init(pNode);

	if( pNode )
	{
		pNode->Release();
	}

	return node;
}

/**
* 파라미터에 해당하는 노드의 Attr 값을 반환
*
* @param	lpszNodeName	  : 찾을 노드
* @param	lpszAttrName	  : 찾을 attr
* @param	lpszDefaultValue  : 기본값
* @return	String 값
*/
CString CXMLWrapperNode::ReadNodeAttrValueString(LPCTSTR lpszNodeName, LPCTSTR lpszAttrName, LPCTSTR lpszDefaultValue)
{
	USES_CONVERSION;

	BSTR bstrExpression;
	CString strNodeValue;
	CXMLWrapperNode node;
	IXMLDOMNode * pNode;
	HRESULT hr;

	bstrExpression	= T2BSTR(lpszNodeName);
	strNodeValue	= lpszDefaultValue;

	hr = m_pNode->selectSingleNode(bstrExpression, &pNode);
	if( SUCCEEDED(hr) )
	{
		node.Init(pNode);

		if( pNode )
		{
			pNode->Release();
		}

		if( node.IsNull() == false )
		{
			node.GetNodeAttrValue(lpszAttrName, strNodeValue);
		}
	}

	::SysFreeString(bstrExpression);

	return strNodeValue;
}

/**
* 파라미터에 해당하는 노드의 Attr 값을 반환
*
* @param	lpszNodeName	  : 찾을 노드
* @param	lpszAttrName	  : 찾을 attr
* @param	iValue  : 기본값
* @return	int 값
*/
int CXMLWrapperNode::ReadNodeAttrValueInt(LPCTSTR lpszNodeName, LPCTSTR lpszAttrName, int iValue)
{
	USES_CONVERSION;

	int iRetValue = 0;
	BSTR bstrExpression;
	CString strNodeValue;
	CXMLWrapperNode node;
	IXMLDOMNode * pNode;
	HRESULT hr;

	bstrExpression	= T2BSTR(lpszNodeName);
	iRetValue		= iValue;

	hr = m_pNode->selectSingleNode(bstrExpression, &pNode);
	if( SUCCEEDED(hr) )
	{
		node.Init(pNode);

		if( pNode )
		{
			pNode->Release();
		}

		if( node.IsNull() == false )
		{
			node.GetNodeAttrValue(lpszAttrName, strNodeValue);
			iRetValue = _ttoi(strNodeValue);
		}
	}

	::SysFreeString(bstrExpression);

	return iRetValue;
}

/**
* 파라미터에 해당하는 노드의 Attr 값을 반환
*
* @param	lpszNodeName	  : 찾을 노드
* @param	lpszAttrName	  : 찾을 attr
* @param	dValue  : 기본값
* @return	double 값
*/
double CXMLWrapperNode::ReadNodeAttrValueFloat(LPCTSTR lpszNodeName, LPCTSTR lpszAttrName, double dValue)
{
	USES_CONVERSION;

	double dRetValue = 0;
	BSTR bstrExpression;
	CString strNodeValue;
	CXMLWrapperNode node;
	IXMLDOMNode * pNode;
	HRESULT hr;

	bstrExpression	= T2BSTR(lpszNodeName);
	dRetValue		= dValue;

	hr = m_pNode->selectSingleNode(bstrExpression, &pNode);
	if( SUCCEEDED(hr) )
	{
		node.Init(pNode);

		if( pNode )
		{
			pNode->Release();
		}

		if( node.IsNull() == false )
		{
			node.GetNodeAttrValue(lpszAttrName, strNodeValue);
			dRetValue = _ttof(strNodeValue);
		}
	}

	::SysFreeString(bstrExpression);

	return dRetValue;
}

/**
* 현재 노드의 도큐먼트를 반환
*/
CXMLWrapperDocument CXMLWrapperNode::GetOwnerDoc()
{
	CComPtr<IXMLDOMDocument> doc;
	HRESULT hr = m_pNode->get_ownerDocument(&doc);
	assert(SUCCEEDED(hr));

	return CXMLWrapperDocument(doc);
}
/**
* 파라미터에 해당하는 노드의 Attr 값을 입력
*
* @param	lpszNodeName	  : 찾을 노드
* @param	lpszAttrName	  : 찾을 attr
* @param	lpszValue  : 입력값
* @return	TRUE / FALSE
*/
BOOL CXMLWrapperNode::WriteNodeAttrValueString(LPCTSTR lpszNodeName, LPCTSTR lpszAttrName, LPCTSTR lpszValue)
{
	USES_CONVERSION;

	HRESULT hr;
	BSTR bstrExpression;
	CString strNodeValue;
	CXMLWrapperNode node;
	IXMLDOMNode * pNode;

	CXMLWrapperDocument wrapperDoc = GetOwnerDoc();

	bstrExpression	= T2BSTR(lpszNodeName);

	hr = m_pNode->selectSingleNode(bstrExpression, &pNode);
	::SysFreeString(bstrExpression);

	if( FAILED(hr) == TRUE )
	{
		node = wrapperDoc.CreateElement(lpszNodeName);

		if( wrapperDoc.InsertNodeAttribute(node, lpszAttrName, lpszValue) == TRUE )
		{
			if( AppendChild(&node) == false )
			{
				return FALSE;
			}
		}
	}
	else
	{
		node.Init(pNode);

		if( pNode )
		{
			pNode->Release();
		}

		if( node.SetNodeAttrValue(lpszAttrName, lpszValue) == FALSE )
		{
			if( wrapperDoc.InsertNodeAttribute(node, lpszAttrName, lpszValue) == FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/**
* 파라미터에 해당하는 노드의 Attr 값을 입력
*
* @param	lpszNodeName	  : 찾을 노드
* @param	lpszAttrName	  : 찾을 attr
* @param	dValue  : 입력값
* @return	TRUE / FALSE
*/
BOOL CXMLWrapperNode::WriteNodeAttrValueFloat(LPCTSTR lpszNodeName, LPCTSTR lpszAttrName, double dValue)
{
	USES_CONVERSION;

	HRESULT hr;
	BSTR bstrExpression;
	CString strNodeValue;
	CXMLWrapperNode node;
	IXMLDOMNode * pNode;

	CXMLWrapperDocument wrapperDoc = GetOwnerDoc();

	bstrExpression	= T2BSTR(lpszNodeName);
	strNodeValue.Format(_T("%.2f"), dValue);

	hr = m_pNode->selectSingleNode(bstrExpression, &pNode);
	::SysFreeString(bstrExpression);

	if( FAILED(hr) == TRUE )
	{
		node = wrapperDoc.CreateElement(lpszNodeName);

		if( wrapperDoc.InsertNodeAttribute(node, lpszAttrName, strNodeValue) == TRUE )
		{
			if( AppendChild(&node) == false )
			{
				return FALSE;
			}
		}
	}
	else
	{
		node.Init(pNode);

		if( pNode )
		{
			pNode->Release();
		}

		if( node.SetNodeAttrValue(lpszAttrName, strNodeValue) == FALSE )
		{
			if( wrapperDoc.InsertNodeAttribute(node, lpszAttrName, strNodeValue) == FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/**
* 파라미터에 해당하는 노드의 Attr 값을 입력
*
* @param	lpszNodeName	  : 찾을 노드
* @param	lpszAttrName	  : 찾을 attr
* @param	iValue  : 입력값
* @return	TRUE / FALSE
*/
BOOL CXMLWrapperNode::WriteNodeAttrValueInt(LPCTSTR lpszNodeName, LPCTSTR lpszAttrName, int iValue)
{
	USES_CONVERSION;

	HRESULT hr;
	BSTR bstrExpression;
	CString strNodeValue;
	CXMLWrapperNode node;
	IXMLDOMNode * pNode;

	CXMLWrapperDocument wrapperDoc = GetOwnerDoc();

	bstrExpression	= T2BSTR(lpszNodeName);

	hr = m_pNode->selectSingleNode(bstrExpression, &pNode);
	::SysFreeString(bstrExpression);

	if( FAILED(hr) == TRUE )
	{
		node = wrapperDoc.CreateElement(lpszNodeName);

		if( wrapperDoc.InsertNodeAttribute(node, lpszAttrName, strNodeValue) == TRUE )
		{
			if( AppendChild(&node) == false )
			{
				return FALSE;
			}
		}
	}
	else
	{
		node.Init(pNode);

		if( pNode )
		{
			pNode->Release();
		}

		if( node.SetNodeAttrValue(lpszAttrName, strNodeValue) == FALSE )
		{
			if( wrapperDoc.InsertNodeAttribute(node, lpszAttrName, strNodeValue) == FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

/**
* 파라미터에 해당하는 child 노드의 값을 반환
*
* @param	lpszChildNode	  : 찾을 child 노드
* @param	strNodeValue	  : [out] child 노드 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::GetChildNodeValue(LPCTSTR lpszChildNode, CString & strNodeValue)
{
	CString strNodeName;
	CXMLWrapperNode nodeChild;
	CXMLWrapperIterator	*pIter;

	pIter = GetChildren();
	if( pIter == NULL )			return FALSE;

	for( pIter->Begin(); pIter->IsDone() == false; pIter->Next() )
	{
		nodeChild = pIter->CurrentItem();
		if( nodeChild.IsNull() == false )
		{
			strNodeName = nodeChild.GetNodeName();

			if( strNodeName == lpszChildNode )
			{
				strNodeValue = nodeChild.GetNodeValue();
				return TRUE;
			}
		}
	}

	return FALSE;
}

/**
* 파라미터에 해당하는 child 노드의 값을 입력
*
* @param	lpszChildNode	  : 찾을 child 노드
* @param	strNodeValue	  : child 노드 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::SetChildNodeValue(LPCTSTR lpszChildNode, LPCTSTR lpszNodeValue)
{
	CString strNodeName;
	CXMLWrapperNode nodeChild;
	CXMLWrapperIterator	*pIter;

	nodeChild = SelectSingleNode(lpszChildNode);

	pIter = nodeChild.GetChildren();
	if( nodeChild.IsNull() == false )
	{
		nodeChild.PutNodeTypedValue(lpszNodeValue);
		return TRUE;
	}

	return FALSE;
}

/**
* 파라미터에 해당하는 child 노드의 값을 입력
*
* @param	lpszChildNode	: 찾을 child 노드
* @param	iNodeValue		: child 노드 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::SetChildNodeValue(LPCTSTR lpszChildNode, int iNodeValue)
{
	CString strNodeName;
	CXMLWrapperNode nodeChild;

	nodeChild = SelectSingleNode(lpszChildNode);

	if( nodeChild.IsNull() == false )
	{
		nodeChild.PutNodeTypedValue(iNodeValue);
		return TRUE;
	}

	return FALSE;
}

/**
* 파라미터에 해당하는 child 노드의 값을 입력
*
* @param	lpszChildNode	: 찾을 child 노드
* @param	lNodeValue		: child 노드 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::SetChildNodeValue(LPCTSTR lpszChildNode, long lNodeValue)
{
	CString strNodeName;
	CXMLWrapperNode nodeChild;

	nodeChild = SelectSingleNode(lpszChildNode);

	if( nodeChild.IsNull() == false )
	{
		nodeChild.PutNodeTypedValue(lNodeValue);
		return TRUE;
	}

	return FALSE;
}

/**
* 파라미터에 해당하는 child 노드의 값을 입력
*
* @param	lpszChildNode	: 찾을 child 노드
* @param	bNodeValue		: child 노드 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::SetChildNodeValue(LPCTSTR lpszChildNode, bool bNodeValue)
{
	CString strNodeName;
	CXMLWrapperNode nodeChild;

	nodeChild = SelectSingleNode(lpszChildNode);

	if( nodeChild.IsNull() == false )
	{
		nodeChild.PutNodeTypedValue(bNodeValue);
		return TRUE;
	}

	return FALSE;
}

/**
* 파라미터에 해당하는 child 노드의 값을 입력
*
* @param	lpszChildNode	: 찾을 child 노드
* @param	dNodeValue		: child 노드 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::SetChildNodeValue(LPCTSTR lpszChildNode, double dNodeValue)
{
	CString strNodeName;
	CXMLWrapperNode nodeChild;

	nodeChild = SelectSingleNode(lpszChildNode);

	if( nodeChild.IsNull() == false )
	{
		nodeChild.PutNodeTypedValue(dNodeValue);
		return TRUE;
	}

	return FALSE;
}

/**
* 파라미터에 해당하는 child 노드의 값을 입력
*
* @param	lpszChildNode	: 찾을 child 노드
* @param	uNodeValue		: child 노드 값
* @return	성공 여부
*/
BOOL CXMLWrapperNode::SetChildNodeValue(LPCTSTR lpszChildNode, UINT uNodeValue)
{
	CString strNodeName;
	CXMLWrapperNode nodeChild;

	nodeChild = SelectSingleNode(lpszChildNode);

	if( nodeChild.IsNull() == false )
	{
		nodeChild.PutNodeTypedValue(uNodeValue);
		return TRUE;
	}

	return FALSE;
}

/**
* 현재 노드의 Attribute 리스트 반환.
*
* @return	Attribute 리스트
*/
CXMLWrapperNodeMap CXMLWrapperNode::GetAttributes()
{
	IXMLDOMNamedNodeMap * pNamedNodeMap = NULL;
	HRESULT hr = m_pNode->get_attributes(&pNamedNodeMap);
	assert(SUCCEEDED(hr));

	CXMLWrapperNodeMap nodeMap;

	nodeMap.Init(pNamedNodeMap);

	if( pNamedNodeMap )
	{
		pNamedNodeMap->Release();
	}

	return nodeMap;
}

////////////////////////////////////////////////////////////////////////////////
//	CXMLWrapperIteratorMaps
////////////////////////////////////////////////////////////////////////////////

/**
* NamedNodeMap을 초기화 함
*
* @param	pNamedNodeMap	: 초기화할 리스트
*/
void CXMLWrapperIteratorMaps::Init(IXMLDOMNamedNodeMap * pNamedNodeMap)
{
	m_pNamedNodeMap = pNamedNodeMap;
}

/**
* Attribute 리스트의 반복자를 시작 위치로 이동.
*
*/
void CXMLWrapperIteratorMaps::Begin()
{
	m_pNamedNodeMap->reset();
	HRESULT hr = m_pNamedNodeMap->get_length(&m_lLength);
	if( FAILED(hr) )
	{
		m_lLength = 0;
	}

	m_lIndex = 0;
}

/**
* Attribute 리스트의 반복자를 다음 위치로 이동.
*
*/
void CXMLWrapperIteratorMaps::Next()
{
	m_lIndex ++;
}

/**
* 현재 반복자에 Attribute가 유효한지 확인.
*
* @return	유효 여부
*/
bool CXMLWrapperIteratorMaps::IsDone()
{
	return (m_lIndex >= m_lLength);
}

/**
* 현재 반복자에 Attribute 노드를 반환
*
* @return	Attribute 노드
*/
CXMLWrapperNode CXMLWrapperIteratorMaps::CurrentItem()
{
	CXMLWrapperNode node;

	IXMLDOMNode * pNode;
	HRESULT hr = m_pNamedNodeMap->get_item(m_lIndex, &pNode);
	assert(SUCCEEDED(hr));

	node.Init(pNode);
	pNode->Release();

	return node;
}

////////////////////////////////////////////////////////////////////////////////
//	CXMLWrapperNodeMap
////////////////////////////////////////////////////////////////////////////////

/**
* CXMLWrapperNodeMap Constructor...
*
*/
CXMLWrapperNodeMap::CXMLWrapperNodeMap()
{
}

/**
* CXMLWrapperNodeMap Constructor...
*
*/
CXMLWrapperNodeMap::CXMLWrapperNodeMap(const CXMLWrapperNodeMap& nodeMap)
{
	(*this) = nodeMap;
}

/**
* CXMLWrapperNodeMap Constructor...
*
*/
CXMLWrapperNodeMap::CXMLWrapperNodeMap(IXMLDOMNamedNodeMap * pNamedNodeMap)
{
	m_pNamedNodeMap = pNamedNodeMap;
}

/**
* CXMLWrapperNodeMap operator = ...
*
*/
void CXMLWrapperNodeMap::operator=(const CXMLWrapperNodeMap& nodeMap)
{
	m_pNamedNodeMap = nodeMap.m_pNamedNodeMap;
}

/**
* CXMLWrapperNodeMap 초기화
*
*/
void CXMLWrapperNodeMap::Init(IXMLDOMNamedNodeMap * pNamedNodeMap)
{
	m_pNamedNodeMap = pNamedNodeMap;
}

/**
* CXMLWrapperNodeMap 종료
*
*/
void CXMLWrapperNodeMap::Term()
{
	m_pNamedNodeMap.Release();
}

/**
* Attribute 리스트의 유효성 확인
*
* @return	유효 여부
*/
bool CXMLWrapperNodeMap::IsNull()
{
	return (m_pNamedNodeMap.p == NULL);
}

/**
* Attribute 리스트 개수 반환
*
* @return	전체 Attribute 개수
*/
long CXMLWrapperNodeMap::GetLength()
{
	if( m_pNamedNodeMap.p == NULL )
	{
		return 0;
	}

	long lLength;
	HRESULT hr = m_pNamedNodeMap->get_length(&lLength);
	assert(SUCCEEDED(hr));

	return lLength;
}

/**
* Attribute 노드 반환
*
* @return	Attribute 노드
*/
CXMLWrapperNode CXMLWrapperNodeMap::Item(LPCTSTR pcszName)
{
	assert(m_pNamedNodeMap.p != NULL);

	USES_CONVERSION;
	BSTR bstrName = T2BSTR(pcszName);

	IXMLDOMNode * pNode;
	HRESULT hr = m_pNamedNodeMap->getNamedItem(bstrName, &pNode);
	assert(SUCCEEDED(hr));

	::SysFreeString(bstrName);

	CXMLWrapperNode node;
	node.Init(pNode);
	pNode->Release();

	return node;
}

/**
* Attribute 노드 반환
*
* @return	Attribute 노드
*/
CXMLWrapperNode CXMLWrapperNodeMap::Item(long lIndex)
{
	assert(m_pNamedNodeMap.p != NULL);

	IXMLDOMNode * pNode;
	HRESULT hr = m_pNamedNodeMap->get_item(lIndex, &pNode);
	assert(SUCCEEDED(hr));

	CXMLWrapperNode node;
	node.Init(pNode);
	pNode->Release();

	return node;
}

BOOL CXMLWrapperNodeMap::ValueOfItem(LPCTSTR pcszName, CString & strNodeValue)
{
	assert(m_pNamedNodeMap.p != NULL);

	USES_CONVERSION;
	BSTR bstrName = T2BSTR(pcszName);

	BOOL bRet = TRUE;
	IXMLDOMNode * pNode;
	VARIANT var;
	HRESULT hr = m_pNamedNodeMap->getNamedItem(bstrName, &pNode);
	assert(SUCCEEDED(hr));

	if (pNode != NULL)
	{
		hr = pNode->get_nodeTypedValue(&var);
		assert(SUCCEEDED(hr));

		if(var.vt == VT_BSTR)
		{
			strNodeValue = OLE2T(var.bstrVal);
			::SysFreeString(var.bstrVal);

			bRet = TRUE;
		}
		else
		{
			bRet = FALSE;
		}

		::SysFreeString(bstrName);
		pNode->Release();
	}
	else
	{
		::SysFreeString(bstrName);
		bRet = FALSE;
	}

	return bRet;
}

/**
* Attribute 노드 추가
*
* @param	pNewNode : 추가할 노드
* @return	노드 추가 성공 여부
*/
bool CXMLWrapperNodeMap::AppendNode(CXMLWrapperNode * pNewNode)
{
	assert(m_pNamedNodeMap.p != NULL);

	IXMLDOMNode* pNamedItem = NULL;
	HRESULT hr = m_pNamedNodeMap->setNamedItem(pNewNode->m_pNode, &pNamedItem);
	assert(SUCCEEDED(hr));

	if( pNamedItem )
	{
		pNamedItem->Release();
	}

	return true;
}

/**
* Attribute 노드 추가
*
* @param	pNewNode : 추가할 노드
* @return	노드 추가 성공 여부
*/
bool CXMLWrapperNodeMap::AppendNode(IXMLDOMNode * pNewNode)
{
	assert(m_pNamedNodeMap.p != NULL);

	IXMLDOMNode* pNamedItem = NULL;
	HRESULT hr = m_pNamedNodeMap->setNamedItem(pNewNode, &pNamedItem);
	assert(SUCCEEDED(hr));

	if( pNamedItem )
	{
		pNamedItem->Release();
	}

	return true;
}

/**
* Attribute 노드 삭제
*
* @param	pcszName : 삭제할 노드
*/
void CXMLWrapperNodeMap::RemoveNode(LPCTSTR pcszName)
{
	assert(m_pNamedNodeMap.p != NULL);

	USES_CONVERSION;
	BSTR bstrName = T2BSTR(pcszName);

	IXMLDOMNode * pOldNode = NULL;
	HRESULT hr = m_pNamedNodeMap->removeNamedItem(bstrName, &pOldNode);
	assert(SUCCEEDED(hr));

	if( pOldNode )
	{
		pOldNode->Release();
	}

	::SysFreeString(bstrName);
}

/**
* 전체 노드를 삭제
*
*/
void CXMLWrapperNodeMap::RemoveAll()
{
	assert(m_pNamedNodeMap.p != NULL);
	m_pNamedNodeMap->reset();
}

/**
* Attribute 노드 반복자를 반환
*
* @return	노드 반복자
*/
CXMLWrapperIterator* CXMLWrapperNodeMap::GetIterator()
{
	m_iterator.Init(m_pNamedNodeMap);
	return &m_iterator;
}