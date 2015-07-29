// SectionEdit.cpp : implementation file
//

#include "stdafx.h"
#include "info_PE.h"
#include "SectionEdit.h"
#include "afxdialogex.h"


// CSectionEdit dialog

IMPLEMENT_DYNAMIC(CSectionEdit, CDialogEx)

CSectionEdit::CSectionEdit(CWnd* pParent /*=NULL*/)
: CDialogEx(CSectionEdit::IDD, pParent)
{

}

CSectionEdit::~CSectionEdit()
{
}

void CSectionEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSectionEdit, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSectionEdit::OnBnClickedOk)
END_MESSAGE_MAP()


// CSectionEdit message handlers


void CSectionEdit::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString szName;
	CString szSize;
	wchar_t *szEnd;
	char szSecName[8] = { 0 };
	CEdit* editS = (CEdit*)GetDlgItem(IDC_SEC_NAME);
	editS->GetWindowTextW(szName);
	editS = (CEdit*)GetDlgItem(IDC_SEC_SIZE);
	editS->GetWindowTextW(szSize);

	if (szName == L"" || szSize == L"")
	{
		CDialogEx::OnOK();
	}
	WideCharToMultiByte(CP_ACP, MB_PRECOMPOSED, szName, 8, szSecName, 8, NULL, NULL);
	size_t dwSecSize = wcstol(szSize, &szEnd, 16);






	CDialogEx::OnOK();
}
