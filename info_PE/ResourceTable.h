#pragma once
#include "afxcmn.h"
#include "MyListCtrl.h"


// CResourceTable dialog

class CResourceTable : public CDialogEx
{
	DECLARE_DYNAMIC(CResourceTable)

public:
	CResourceTable(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResourceTable();

	DWORD dwSize = 0;
	PVOID lpFileImage = nullptr;
	BOOL isx64 = FALSE;

	PIMAGE_RESOURCE_DIRECTORY pBase = nullptr;

	void getResourceTable(HTREEITEM hNode,PIMAGE_RESOURCE_DIRECTORY pRes,PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry, BOOL isList,DWORD dwCount,BOOL isRoot);

	wchar_t* getTypeName(DWORD dwID);

	ULONGLONG RVA2Offset(ULONGLONG rVA, PVOID lpImage, DWORD dwSize);

// Dialog Data
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ResourceTree;
	CMyListCtrl m_ResourceItems;
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
