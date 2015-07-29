
// info_PEDlg.h : header file
//

#pragma once
#include "MyListCtrl.h"


// Cinfo_PEDlg dialog
class Cinfo_PEDlg : public CDialogEx
{
	// Construction
public:
	Cinfo_PEDlg(CWnd* pParent = NULL);	// standard constructor
	friend class CCalc;
	// Dialog Data
	enum { IDD = IDD_INFO_PE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	void getPEinfo(LPCTSTR szFileName);
	BOOL PE_fileTest(PVOID lpImage, DWORD dwSize);
	void showPE_info(PVOID lpImage, DWORD dwSize);

	BOOL isx64 = FALSE;

	wchar_t szFile[MAX_PATH];
	HANDLE hFile = nullptr;
	CMyListCtrl m_ExtendedHeader;
	CMyListCtrl m_DataDir;
	CMyListCtrl m_SectionInfo;
	DWORD dwSize = 0;
	PVOID lpFileImage = nullptr;
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCalc32771();
	virtual BOOL DestroyWindow();
	afx_msg void OnCalc32772();
	afx_msg void OnCalc32773();
	afx_msg void OnCalc32774();
	afx_msg void OnCalc32775();
	afx_msg void OnCalc32776();
	afx_msg void OnDestroy();
};
