#pragma once
#include "MyListCtrl.h"


// CRelocationTable dialog

typedef struct _TYPE_OFFSET
{
	WORD Offset : 12;
	WORD Type : 4;
}TYPE_OFFSET, *PTYPE_OFFSET;

class CRelocationTable : public CDialogEx
{
	DECLARE_DYNAMIC(CRelocationTable)

public:
	CRelocationTable(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRelocationTable();

	void getRelocTable(PVOID lpBuf, DWORD dwSize);

	DWORD dwSize = 0;
	PVOID lpFileImage = nullptr;
	BOOL isx64 = FALSE;

	ULONGLONG RVA2Offset(ULONGLONG rVA, PVOID lpImage, DWORD dwSize);

	wchar_t* getSection(ULONGLONG rVA, PVOID lpImage, DWORD dwSize);

	void getRelocItems(ULONGLONG ullRVA, DWORD dwCount, PVOID lpBuf, DWORD dwSize);

	wchar_t* getRelocType(DWORD dwType);


	// Dialog Data
	enum { IDD = IDD_DIALOG5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_blockTable;
	CMyListCtrl m_itemTable;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickList8(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownList8(NMHDR *pNMHDR, LRESULT *pResult);
};
