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
* @param	ptr : IXMLDomDocument ����Ʈ ������.
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
* CXMLWrapperDocument �ʱ�ȭ. CLSID_DOMDocument COM ��ü�� ������.
*
*/
void CXMLWrapperDocument::Init()
{
	HRESULT hr = m_document.CoCreateInstance(CLSID_DOMDocument);
}

/**
* InitIndent
* Indent ���� �˻� �� �ʱ�ȭ
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
* Document ��ä�� ��������� Release.(����ī���Ͱ���).
*
*/
void CXMLWrapperDocument::Term()
{
	m_document.Release();
}

/**
* XML ������ Document��ü�� Load.
*
* @param	pcszFileName : XML ������ Path
* @return	XML���� Open ���� ����
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
* iStream ��ü�� ���� XML ������ Document��ü�� Load.
*
* @param	pStream : IStream ��ü
* @return	XML���� Open ���� ����
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
* XML ������ Document��ü�� Load.
*
* @param	bstrName : XML ���� ���(BSTR Type)
* @return	XML���� Open ���� ����
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
* XML Document��ü�� ����� �Ϸ� �� Document�� Close �ϴ� �Լ�.
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
* XML Wrapper�� Document ��ü�� ��ü�ϴ� �Լ�.
*
* @param	newDocument : ���� ��ü�� document
*/
void CXMLWrapperDocument::Replace(CComPtr<IXMLDOMDocument>& newDocument)
{
	m_document.Release();

	m_document = newDocument;

	m_szFileName[0] = _T('\0');
}

/**
* XML ������ �����ϴ� �Լ�.
*
* @param	pcszFileName : XML������ ������ ���ϸ�
* @return	XML���� ���� ���� ����
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
* XML Document���� child ����� ����Ʈ�� �����´�.
*
* @return	child ��� ����Ʈ�� �ݺ���.
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
* XML Document�� ������Ƽ�� ����.
*
* @param	pcszKey		: ������Ƽ Ű
* @param	pcszValue	: ������Ƽ ��
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
* XML Document�� ������Ƽ�� ������.
*
* @param	pcszKey	: ������ ������Ƽ Ű
* @return	������Ƽ�� ��
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
* Element�� ����.
*
* @param	pcszTagName	: Element �ױ׸�
* @return	������ Element ���
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
* Text ��带 ����.
*
* @param	pcszTextNode : ǥ���� ���ڿ�
* @return	������ �ؽ�Ʈ ���
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
* Attribute ��带 ����.
*
* @param	pcszAttrName : Attribute �̸�
* @return	������ Attribute ���
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
* Attribute ��带 ����.
*
* @param	node : �Ӽ���� Parent
* @param	pcszAttrName : Attribute �̸�
* @param	lpszAttrValue : Attribute ��
* @return	���� / ����
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
* Attribute ��带 ����.
*
* @param	node : �Ӽ���� Parent
* @param	pcszAttrName : Attribute �̸�
* @param	lpszAttrValue : Attribute ��
* @return	���� / ����
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
* Entity ��带 ����.
*
* @param	pcszEntityName : Entity �̸�
* @return	������ Entity ���
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
* Fragment ��带 ����.
*
* @return	������ Fragment ���
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
* ������ ��忡 Attribute�� �Է���
*
* @param	node			: Attribute�� �Է��� ���
* @param	pcszAttrName	: Attribute �̸�
* @param	pcszAttrValue	: Attribute ��
* @return	�Է� ���� ����
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
* XML ���� Load, Save�� ������ ó���ϴ� ����� ����
*
* @param	bPreserving : true �������� / false ��ȿ���鸸 ����
*/
void CXMLWrapperDocument::PreServeWhiteSpace(bool bPreserving)
{
	m_document->put_preserveWhiteSpace(bPreserving ? VARIANT_TRUE : VARIANT_FALSE);
}

/**
* �ֻ���(ROOT) ��带 ������
*
* @return	root ���
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
* Document�� Element �Է�.
*
* @param	pNode : �Է��� Element ���
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
* Document�� Element ������.
*
* @return	Element ���
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
* Document�� ����Ʈ ������ ��������.
*
*/
CComPtr<IXMLDOMDocument> CXMLWrapperDocument::GetDocument()
{
	return m_document;
}

/**
* IndentDocument
* XSL�̿� XML ���� �鿩����
* @param CString strInputXmlPath : XML ���
* @return	���� / ����
*/
BOOL CXMLWrapperDocument::IndentDocument(CString strInputXmlPath)
{
	// ���� ��ü, �����ü����
	CComPtr<IXMLDOMDocument> pXML = NULL, pXSL = NULL, pResult = NULL; // ��������, // ��Ÿ�Ͻ�Ʈ����, // �����Ұ������
	HRESULT hr = S_OK;
	VARIANT vObject, var;
	VARIANT_BOOL bSucessful;

	USES_CONVERSION;

	var.vt		= VT_BSTR;
	var.bstrVal = T2BSTR(strInputXmlPath);

	// ���� XML ���Ϻҷ�����
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

	// ��Ÿ�Ͻ�Ʈ���� �ҷ�����
	pXSL.CoCreateInstance(__uuidof(DOMDocument));
	pXSL->put_async(VARIANT_FALSE);
	hr = pXSL->load(var, &bSucessful); // XSL ���� ���

	::SysFreeString(var.bstrVal);

	if ( hr == S_FALSE )
	{
		OutputDebugString(_T("XSL File Load Error"));
		return FALSE;
	}

	// ������ϰ�ü�ʱ�ȭ
	pResult.CoCreateInstance(__uuidof(DOMDocument));

	// ��ȯ ���ú����������ʱ�ȭ
	VariantInit(&vObject);
	vObject.vt = VT_DISPATCH;
	vObject.pdispVal = pResult;

	// ��ȯ ����
	hr = pXML->transformNodeToObject(pXSL, vObject);
	if ( hr == E_FAIL )
	{
		OutputDebugString(_T("Transformation Error"));
		return FALSE;
	}

	// ��ȯ������ �����ϱ�
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
* �Էµ� ��� ����Ʈ�� �ʱ�ȭ
*
* @param	pNodeList : �Է��� ��� ����Ʈ
*/
void CXMLWrapperIteratorNodes::Init(IXMLDOMNodeList * pNodeList)
{
	m_pNodeList = pNodeList;
}

/**
* ��� ����Ʈ�� ���� ��ġ�� �ݺ��ڸ� �̵�.
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
* ��� ����Ʈ�� ���� �ݺ��� ��ġ�� �̵�.
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
* ���� ������ �ݺ��ڰ� ��ȿ���� Ȯ��.
*
* @return	�ݺ����� ��ȿ��
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
* ���� ������ �ݺ����� ��带 ������.
*
* @return	���� ������ �ݺ����� ���.
*/
CXMLWrapperNode CXMLWrapperIteratorNodes::CurrentItem()
{
	return (*m_pNode);
}

/**
* ����� �� ������ ������
*
* @return	����� ����
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
* ����� ��ȿ���� Ȯ��.
*
* @return	����� ��ȿ��.
*/
bool CXMLWrapperNode::IsNull()
{
	return (m_pNode.p == NULL);
}

/**
* ����� ������ Ÿ���� ��ȯ
*
* @return	����� ������ Ÿ��
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
* ����� ������ Ÿ���� �Է�
*
* @param	pcszType : �Է��� ������ Ÿ��
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
* ����� �ױ��̸��� ��ȯ
*
* @return	����� �ױ� �̸�
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
* ���� ����� child����� �ݺ��ڸ� ��ȯ.
*
* @return	child����� �ݺ���
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
* �ε����� �ش��ϴ� child��带 ��ȯ
*
* @param	lIndex : ã�� ����� �ε���
* @return	child���
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
* ����� Ÿ�� ��ȯ
*
* @return	���Ÿ��
*/
DOMNodeType CXMLWrapperNode::GetNodeType()
{
	DOMNodeType domNodeType;

	HRESULT hr = m_pNode->get_nodeType(&domNodeType);
	assert(SUCCEEDED(hr));

	return domNodeType;
}

/**
* ����� ���� ��ȯ
*
* @param	Int2Type<VT_BSTR> : BSTR Ÿ���� ������ �ð��� ����
* @return	��� ��.
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
* ����� ���� ��ȯ
*
* @param	Int2Type<VT_I4> : long Ÿ���� ������ �ð��� ����
* @return	��� ��.
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
* ����� ���� ��ȯ
*
* @param	Int2Type<VT_BOOL> : bool Ÿ���� ������ �ð��� ����
* @return	��� ��.
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
* ����� ���� ��ȯ
*
* @param	Int2Type<VT_R8> : double Ÿ���� ������ �ð��� ����
* @return	��� ��.
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
* ����� ���� �Է�
*
* @param	pcszValue : �Է� ���ڿ�
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
* ����� ���� �Է�
*
* @param	lValue : long ��
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
* ����� ���� �Է�
*
* @param	iValue : int ��
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
* ����� ���� �Է�
*
* @param	uValue : unsigned int ��
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
* ����� ���� �Է�
*
* @param	bValue : bool ��
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
* ����� ���� �Է�
*
* @param	dValue : double ��
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
* ����� ���� ��ȯ
*
* @return	����� ��
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
* ����� ���� ��ȯ
*
* @param	pcszAttrName : Attribute �̸�
* @param	strAttrValue : [out] Attribute ��
* @return	���� ����
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
* ����� ���� ����
*
* @param	pcszAttrName : Attribute �̸�
* @param	strAttrValue : Attribute ��
* @return	���� ����
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
* ��忡 ���ڿ� ���� �Է�
*
* @param	pcszValue : �Է� ��
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
* ���� ��忡 child ��带 �߰�
*
* @param	pNode	  : �߰��� child ���
* @return	���� ����
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
* ���� ��尡 child ��带 ������ �ִ��� Ȯ��
*
* @return	child ��� ����
*/
bool CXMLWrapperNode::HasChildNodes()
{
	VARIANT_BOOL varBool;

	HRESULT hr = m_pNode->hasChildNodes(&varBool);
	assert(SUCCEEDED(hr));

	return (varBool == VARIANT_TRUE);
}

/**
* ���� ��忡�� ������ child ��带 ����
*
* @param	pNode	  : ������ child ���
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
* ���� ��忡�� ������ child�� ��ü
*
* @param	pNewNode	  : ���� ������ child ���
* @param	pOldNode	  : ���� ��� child ���
* @return	��ä ���� ����
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
* �Ķ���Ϳ� �ش��ϴ� ��� ����Ʈ�� ��ȯ
*
* @param	pcszExpression	  : ã�� ����� ����Ʈ
* @return	��� ����Ʈ �ݺ���
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
* �Ķ���Ϳ� �ش��ϴ� ��带 ��ȯ
*
* @param	pcszExpression	  : ã�� ���
* @return	���
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
* �Ķ���Ϳ� �ش��ϴ� ����� Attr ���� ��ȯ
*
* @param	lpszNodeName	  : ã�� ���
* @param	lpszAttrName	  : ã�� attr
* @param	lpszDefaultValue  : �⺻��
* @return	String ��
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
* �Ķ���Ϳ� �ش��ϴ� ����� Attr ���� ��ȯ
*
* @param	lpszNodeName	  : ã�� ���
* @param	lpszAttrName	  : ã�� attr
* @param	iValue  : �⺻��
* @return	int ��
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
* �Ķ���Ϳ� �ش��ϴ� ����� Attr ���� ��ȯ
*
* @param	lpszNodeName	  : ã�� ���
* @param	lpszAttrName	  : ã�� attr
* @param	dValue  : �⺻��
* @return	double ��
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
* ���� ����� ��ť��Ʈ�� ��ȯ
*/
CXMLWrapperDocument CXMLWrapperNode::GetOwnerDoc()
{
	CComPtr<IXMLDOMDocument> doc;
	HRESULT hr = m_pNode->get_ownerDocument(&doc);
	assert(SUCCEEDED(hr));

	return CXMLWrapperDocument(doc);
}
/**
* �Ķ���Ϳ� �ش��ϴ� ����� Attr ���� �Է�
*
* @param	lpszNodeName	  : ã�� ���
* @param	lpszAttrName	  : ã�� attr
* @param	lpszValue  : �Է°�
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
* �Ķ���Ϳ� �ش��ϴ� ����� Attr ���� �Է�
*
* @param	lpszNodeName	  : ã�� ���
* @param	lpszAttrName	  : ã�� attr
* @param	dValue  : �Է°�
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
* �Ķ���Ϳ� �ش��ϴ� ����� Attr ���� �Է�
*
* @param	lpszNodeName	  : ã�� ���
* @param	lpszAttrName	  : ã�� attr
* @param	iValue  : �Է°�
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
* �Ķ���Ϳ� �ش��ϴ� child ����� ���� ��ȯ
*
* @param	lpszChildNode	  : ã�� child ���
* @param	strNodeValue	  : [out] child ��� ��
* @return	���� ����
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
* �Ķ���Ϳ� �ش��ϴ� child ����� ���� �Է�
*
* @param	lpszChildNode	  : ã�� child ���
* @param	strNodeValue	  : child ��� ��
* @return	���� ����
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
* �Ķ���Ϳ� �ش��ϴ� child ����� ���� �Է�
*
* @param	lpszChildNode	: ã�� child ���
* @param	iNodeValue		: child ��� ��
* @return	���� ����
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
* �Ķ���Ϳ� �ش��ϴ� child ����� ���� �Է�
*
* @param	lpszChildNode	: ã�� child ���
* @param	lNodeValue		: child ��� ��
* @return	���� ����
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
* �Ķ���Ϳ� �ش��ϴ� child ����� ���� �Է�
*
* @param	lpszChildNode	: ã�� child ���
* @param	bNodeValue		: child ��� ��
* @return	���� ����
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
* �Ķ���Ϳ� �ش��ϴ� child ����� ���� �Է�
*
* @param	lpszChildNode	: ã�� child ���
* @param	dNodeValue		: child ��� ��
* @return	���� ����
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
* �Ķ���Ϳ� �ش��ϴ� child ����� ���� �Է�
*
* @param	lpszChildNode	: ã�� child ���
* @param	uNodeValue		: child ��� ��
* @return	���� ����
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
* ���� ����� Attribute ����Ʈ ��ȯ.
*
* @return	Attribute ����Ʈ
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
* NamedNodeMap�� �ʱ�ȭ ��
*
* @param	pNamedNodeMap	: �ʱ�ȭ�� ����Ʈ
*/
void CXMLWrapperIteratorMaps::Init(IXMLDOMNamedNodeMap * pNamedNodeMap)
{
	m_pNamedNodeMap = pNamedNodeMap;
}

/**
* Attribute ����Ʈ�� �ݺ��ڸ� ���� ��ġ�� �̵�.
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
* Attribute ����Ʈ�� �ݺ��ڸ� ���� ��ġ�� �̵�.
*
*/
void CXMLWrapperIteratorMaps::Next()
{
	m_lIndex ++;
}

/**
* ���� �ݺ��ڿ� Attribute�� ��ȿ���� Ȯ��.
*
* @return	��ȿ ����
*/
bool CXMLWrapperIteratorMaps::IsDone()
{
	return (m_lIndex >= m_lLength);
}

/**
* ���� �ݺ��ڿ� Attribute ��带 ��ȯ
*
* @return	Attribute ���
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
* CXMLWrapperNodeMap �ʱ�ȭ
*
*/
void CXMLWrapperNodeMap::Init(IXMLDOMNamedNodeMap * pNamedNodeMap)
{
	m_pNamedNodeMap = pNamedNodeMap;
}

/**
* CXMLWrapperNodeMap ����
*
*/
void CXMLWrapperNodeMap::Term()
{
	m_pNamedNodeMap.Release();
}

/**
* Attribute ����Ʈ�� ��ȿ�� Ȯ��
*
* @return	��ȿ ����
*/
bool CXMLWrapperNodeMap::IsNull()
{
	return (m_pNamedNodeMap.p == NULL);
}

/**
* Attribute ����Ʈ ���� ��ȯ
*
* @return	��ü Attribute ����
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
* Attribute ��� ��ȯ
*
* @return	Attribute ���
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
* Attribute ��� ��ȯ
*
* @return	Attribute ���
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
* Attribute ��� �߰�
*
* @param	pNewNode : �߰��� ���
* @return	��� �߰� ���� ����
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
* Attribute ��� �߰�
*
* @param	pNewNode : �߰��� ���
* @return	��� �߰� ���� ����
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
* Attribute ��� ����
*
* @param	pcszName : ������ ���
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
* ��ü ��带 ����
*
*/
void CXMLWrapperNodeMap::RemoveAll()
{
	assert(m_pNamedNodeMap.p != NULL);
	m_pNamedNodeMap->reset();
}

/**
* Attribute ��� �ݺ��ڸ� ��ȯ
*
* @return	��� �ݺ���
*/
CXMLWrapperIterator* CXMLWrapperNodeMap::GetIterator()
{
	m_iterator.Init(m_pNamedNodeMap);
	return &m_iterator;
}