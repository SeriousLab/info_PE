#pragma once
#include "MyListCtrl.h"


// CExportTable dialog

class CExportTable : public CDialogEx
{
	DECLARE_DYNAMIC(CExportTable)

public:
	CExportTable(CWnd* pParent = NULL);   // standard constructor
	virtual ~CExportTable();

	PVOID lpImage = nullptr;
	DWORD dwSize = 0;
	BOOL isx64 = FALSE;
	ULONGLONG RVA2Offset(ULONGLONG rVA, PVOID lpImage, DWORD dwSize);

	void PrintExport(PVOID lpImage, DWORD dwSize);
	// Dialog Data
	enum { IDD = IDD_DIALOG2 };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CMyListCtrl m_ExportTable;
	virtual BOOL OnInitDialog();
};
