/**
* @file	XMLWrapper.h
*
* @brief	MSXML ���� Ŭ����
* @date	2012.10.10
* @author	YSJung
*/

#if !defined(AFX_XMLWRAPPER_H__1B878487_D88A_4372_9F31_5BEA8C60A033__INCLUDED_)
#define AFX_XMLWRAPPER_H__1B878487_D88A_4372_9F31_5BEA8C60A033__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STR_INDENT_PATH _T("C:\\temp")
#define STR_INDENT_FILENAME _T("Indent.xsl")

#pragma comment (lib, "msxml2.lib")

#include <SDKDDKVer.h>
#include <afx.h>
#include <atlstr.h>
#include <atltrace.h>
#include <msxml2.h>

using namespace ATL;
/**
* @class	Int2Type
*
* @brief	������ Ÿ�ӿ� �Լ� �����ε��� �̿��Ͽ� �б�.
* @remarks	������ Ÿ�ӿ� �־��� ����� ���� �������� �Լ� �� �Ѱ��� ȣ�� �ϴ� ���.
* @date	2012.10.10
* @author	YSJung
*/
template<int v>
struct Int2Type
{
	enum { value = v };
};

//////////////////////////////////////////////////////////////////////
// Class forwarding
class CXMLWrapperNode;
class CXMLWrapperDocument;

/**
* @class	CXMLWrapperIterator
*
* @brief	IXMLDOMNodeList Interface.
* @remarks	IXMLDOMNodeList Wrapper Ŭ���� Interface.
* @date	2012.10.10
* @author	YSJung
*/
class CXMLWrapperIterator
{
public:
	virtual ~CXMLWrapperIterator() {}
	virtual void Begin()					= 0L;
	virtual void Next()						= 0L;
	virtual bool IsDone()					= 0L;
	virtual CXMLWrapperNode CurrentItem()	= 0L;
};

/**
* @class	CXMLWrapperIteratorNodes
*
* @brief	CXMLWrapperIterator�� Implement
* @remarks	IXMLDOMNodeList Wrapper Ŭ������ ��� �޾� ����.
* @date	2012.10.10
* @author	YSJung
*/
class CXMLWrapperIteratorNodes : public CXMLWrapperIterator
{
public:
	CXMLWrapperIteratorNodes();
	~CXMLWrapperIteratorNodes();

	void Init(IXMLDOMNodeList * pNodeList);
	virtual void Begin();
	virtual void Next();
	virtual bool IsDone();
	virtual CXMLWrapperNode CurrentItem();
	virtual LONG GetNodeCount();

private:

	CComPtr<IXMLDOMNodeList>			m_pNodeList;	/////< XML ��� ����Ʈ�� ���� ����Ʈ ������.

	CXMLWrapperNode *					m_pNode;		/////< ���� ���õ� ����� ������.
};

/**
* @class	CXMLWrapperNode
*
* @brief	IXMLDOMNode�� Wrapper.
* @remarks	IXMLDOMNode�� Wrapper Ŭ����.
* @date	2012.10.10
* @author	YSJung
*/
class CXMLWrapperNode
{
public:

	friend class CXMLWrapperNodeMap;
	friend class CXMLWrapperDocument;

	CXMLWrapperNode();
	CXMLWrapperNode(const CXMLWrapperNode& node);
	CXMLWrapperNode(IXMLDOMNode * pNode);

	void operator=(const CXMLWrapperNode& node);

	void Init(IXMLDOMNode* pNode);
	void Term();
	bool IsNull();

	CXMLWrapperIterator* GetChildren();

	CXMLWrapperNode GetChild(long lIndex);
	long GetChildCount();

	CXMLWrapperNodeMap GetAttributes();

	CString GetDataType();
	void PutDataType(LPCTSTR n);

	CString GetNodeName();

	DOMNodeType GetNodeType();

	CString GetNodeTypedValue(Int2Type<VT_BSTR>);
	long	GetNodeTypedValue(Int2Type<VT_I4>);
	bool	GetNodeTypedValue(Int2Type<VT_BOOL>);
	double	GetNodeTypedValue(Int2Type<VT_R8>);

	void PutNodeTypedValue(LPCTSTR pcszValue);
	void PutNodeTypedValue(int iValue);
	void PutNodeTypedValue(long lValue);
	void PutNodeTypedValue(bool bValue);
	void PutNodeTypedValue(double dValue);
	void PutNodeTypedValue(UINT uValue);

	CString GetNodeValue();
	BOOL GetNodeAttrValue(LPCTSTR pcszAttrName, CString & strAttrValue);
	BOOL SetNodeAttrValue(LPCTSTR pcszAttrName, LPCTSTR pcszAttrValue);
	void PutNodeValue(LPCTSTR pcszValue);

	bool AppendChild(CXMLWrapperNode * pNode);
	bool HasChildNodes();
	void RemoveChild(CXMLWrapperNode* n);
	bool ReplaceChild(CXMLWrapperNode* newone, CXMLWrapperNode* oldone);
	CXMLWrapperIterator* SelectNodes(LPCTSTR expression);
	CXMLWrapperNode SelectSingleNode(LPCTSTR expression);
	BOOL GetChildNodeValue(LPCTSTR lpszChildNode, CString & strNodeValue);
	BOOL SetChildNodeValue(LPCTSTR lpszChildNode, LPCTSTR lpszNodeValue);
	BOOL SetChildNodeValue(LPCTSTR lpszChildNode, int iNodeValue);
	BOOL SetChildNodeValue(LPCTSTR lpszChildNode, long lNodeValue);
	BOOL SetChildNodeValue(LPCTSTR lpszChildNode, bool bNodeValue);
	BOOL SetChildNodeValue(LPCTSTR lpszChildNode, double dNodeValue);
	BOOL SetChildNodeValue(LPCTSTR lpszChildNode, UINT uNodeValue);

	CString ReadNodeAttrValueString(LPCTSTR lpszNode, LPCTSTR lpszAttrName, LPCTSTR lpszDefaultValue);
	int ReadNodeAttrValueInt(LPCTSTR lpszNodeName, LPCTSTR lpszAttrName, int iValue);
	double ReadNodeAttrValueFloat(LPCTSTR lpszNodeName, LPCTSTR lpszAttrName, double dValue);

	BOOL WriteNodeAttrValueString(LPCTSTR lpszNode, LPCTSTR lpszAttrName, LPCTSTR lpszValue);
	BOOL WriteNodeAttrValueFloat(LPCTSTR lpszNode, LPCTSTR lpszAttrName, double dValue);
	BOOL WriteNodeAttrValueInt(LPCTSTR lpszNode, LPCTSTR lpszAttrName, int iValue);

	CXMLWrapperDocument GetOwnerDoc();
private:

	CComPtr<IXMLDOMNode>			m_pNode;				/////< XML ����� ����Ʈ ������.
	CXMLWrapperIteratorNodes		m_iteratorNodes;		/////< XML ����� �ݺ���.
};

/**
* @class	CXMLWrapperDocument
*
* @brief	IXMLDOMDocument�� Wrapper.
* @remarks	IXMLDOMDocument�� Wrapper Ŭ����.
* @date	2012.10.10
* @author	YSJung
*/
class CXMLWrapperDocument
{
public:

	CXMLWrapperDocument(CComPtr<IXMLDOMDocument>& ptr);
	CXMLWrapperDocument();
	~CXMLWrapperDocument();

	void Init();
	void InitIndent();										/////< Indent ���� �˻� �� �ʱ�ȭ
	void Term();

	bool Open(LPCTSTR pcszFileName);
	bool Open(CComPtr<IStream> pStream);
	bool Open(BSTR bstrName);
	void Close();

	void Replace(CComPtr<IXMLDOMDocument>& newDocument);
	bool Save(LPCTSTR pcszFileName);

	CXMLWrapperIterator* GetChildren();

	void SetProperty(LPCTSTR pcszKey, LPCTSTR pcszValue);
	CString GetProperty(LPCTSTR pcszKey);

	CXMLWrapperNode CreateElement(LPCTSTR pcszTagName);
	CXMLWrapperNode CreateTextNode(LPCTSTR pcszTextNode);
	CXMLWrapperNode CreateAttribute(LPCTSTR pcszAttrName);
	CXMLWrapperNode CreateEntityReference(LPCTSTR pcszEntityName);
	CXMLWrapperNode CreateDocumentFragment();

	BOOL CreateAttribute(CXMLWrapperNode & node, LPCTSTR pcszAttrName, LPCTSTR lpszAttrValue);
	BOOL CreateAttribute(IXMLDOMNamedNodeMap * pNamedNodeMap, LPCTSTR pcszAttrName, LPCTSTR lpszAttrValue);

	BOOL InsertNodeAttribute(CXMLWrapperNode & node, LPCTSTR lpszAttrName, LPCTSTR lpszAttrValue);

	void PreServeWhiteSpace(bool bPreserving);

	CXMLWrapperNode GetRootNode();
	CXMLWrapperNode* GetRootNodePtr();

	void PutRefDocumentElement(CXMLWrapperNode* pNode);
	CXMLWrapperNode GetDocumentElement();

	CComPtr<IXMLDOMDocument>	GetDocument();
	BOOL IndentDocument(CString strInputXmlPath);	/////< XSL�̿� XML ���� �鿩����

private:

	TCHAR	m_szFileName[MAX_PATH];						/////<

	CComPtr<IXMLDOMDocument>	m_document;				/////< MSXML�� �ֻ��� ��ü(Document).

	CXMLWrapperIteratorNodes	m_iterator;				/////< Document �� �ش��ϴ� XML Nodes �� �ݺ���.

	CString						m_strIndentPath;		/////< XSL ���� ���
};

/**
* @class	CXMLWrapperIteratorMaps
*
* @brief	IXMLDOMNamedNodeMap iterator.
* @remarks	IXMLDOMNamedNodeMap iterator Wrapper Ŭ����.
* @date	2012.10.10
* @author	YSJung
*/
class CXMLWrapperIteratorMaps : public CXMLWrapperIterator
{
public:

	void Init(IXMLDOMNamedNodeMap * pNamedNodeMap);

	virtual void Begin();
	virtual void Next();
	virtual bool IsDone();
	virtual CXMLWrapperNode CurrentItem();

private:

	long							m_lLength;				/////< XML Attribute ����
	long							m_lIndex;				/////< XML Attribute ����Ʈ�� ���� �ε���

	CComPtr<IXMLDOMNamedNodeMap>	m_pNamedNodeMap;		/////< NamedNodeMap ����Ʈ ������(XML Attribute)
};

/**
* @class	CXMLWrapperIteratorMaps
*
* @brief	IXMLDOMNamedNodeMap�� Wrapper.
* @remarks	�Ӽ� ��带 ó���ϴ� Wrapper Ŭ����.
* @date	2012.10.10
* @author	YSJung
*/
class CXMLWrapperNodeMap
{
public:

	CXMLWrapperNodeMap();
	CXMLWrapperNodeMap(const CXMLWrapperNodeMap& NodeMap);
	CXMLWrapperNodeMap(IXMLDOMNamedNodeMap* pNamedNodeMap);
	void operator=(const CXMLWrapperNodeMap& NodeMap);

	void Init(IXMLDOMNamedNodeMap * pNamedNodeMap);
	void Term();
	bool IsNull();
	long GetLength();

	CXMLWrapperNode Item(LPCTSTR pcszName);
	CXMLWrapperNode Item(long lIndex);
	BOOL ValueOfItem(LPCTSTR pcszName, CString & strNodeValue);

	bool AppendNode(CXMLWrapperNode* pNewNode);
	bool AppendNode(IXMLDOMNode * pNewNode);
	void RemoveNode(LPCTSTR pcszName);
	void RemoveAll();

	CXMLWrapperIterator* GetIterator();

private:

	CComPtr<IXMLDOMNamedNodeMap>				m_pNamedNodeMap;	/////< NamedNodeMap ����Ʈ ������(XML Attribute).

	CXMLWrapperIteratorMaps						m_iterator;			/////< NamedNodeMap �ݺ���(Attribute ����Ʈ).
};

#endif	//	_XMLWRAPPER_H_
