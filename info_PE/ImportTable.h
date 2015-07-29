#pragma once
#include "MyListCtrl.h"


// CImportTable dialog

class CImportTable : public CDialogEx
{
	DECLARE_DYNAMIC(CImportTable)

public:
	CImportTable(CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportTable();

	void printImportDLL(PVOID lpBuf, DWORD dwSize);

	ULONGLONG RVA2Offset(ULONGLONG rVA, PVOID lpImage, DWORD dwSize);

	void printImportFunctions(ULONGLONG iAT, ULONGLONG iNT,BOOL isIAT);
// Dialog Data
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_importDLL;
	CMyListCtrl m_importFunctions;
	BOOL isx64 = FALSE;
	DWORD dwSize = 0;
	PVOID lpFileImage = nullptr;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickList5(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnLvnKeydownList5(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownList5(NMHDR *pNMHDR, LRESULT *pResult);
};
