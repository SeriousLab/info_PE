// Calc.cpp : implementation file
//

#include "stdafx.h"
#include "info_PE.h"
#include "Calc.h"
#include "afxdialogex.h"


// CCalc dialog

IMPLEMENT_DYNAMIC(CCalc, CDialogEx)

CCalc::CCalc(CWnd* pParent /*=NULL*/)
: CDialogEx(CCalc::IDD, pParent)
{

}

CCalc::~CCalc()
{
}

void CCalc::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCalc, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON2, &CCalc::OnBnClickedButton2)
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON1, &CCalc::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CCalc::OnBnClickedButton3)
	ON_EN_CHANGE(IDC_EDIT2, &CCalc::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT1, &CCalc::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT3, &CCalc::OnEnChangeEdit3)
END_MESSAGE_MAP()


// CCalc message handlers


void CCalc::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	clearMessage();
	CEdit* edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT1);
	edit_value->EnableWindow(FALSE);
	edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT3);
	edit_value->EnableWindow(FALSE);
	edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT2);
	edit_value->EnableWindow(TRUE);
}


void CCalc::clearMessage()
{
	this->SetDlgItemTextW(IDC_EDIT1, L"");
	this->SetDlgItemTextW(IDC_EDIT2, L"");
	this->SetDlgItemTextW(IDC_EDIT3, L"");
}

// void CCalc::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
// {
// 	// TODO: Add your message handler code here and/or call default
// 
// 	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
// }

void CCalc::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	clearMessage();
	CEdit* edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT2);
	edit_value->EnableWindow(FALSE);
	edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT3);
	edit_value->EnableWindow(FALSE);
	edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT1);
	edit_value->EnableWindow(TRUE);
}




void CCalc::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	clearMessage();
	CEdit* edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT1);
	edit_value->EnableWindow(FALSE);
	edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT2);
	edit_value->EnableWindow(FALSE);
	edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT3);
	edit_value->EnableWindow(TRUE);
}


void CCalc::OnEnChangeEdit2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.


	CString szRVA;
	wchar_t *endPoint;
	CEdit* edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT2);

	if (!edit_value->IsWindowEnabled())
	{
		return;
	}

	edit_value->GetWindowTextW(szRVA);
	ULONGLONG ullRVA = wcstol(szRVA, &endPoint, 16);
	if (ullRVA == 0)
	{
		return;
	}
	ULONGLONG ullOffset = RVA2Offset(ullRVA, lpFileImage, dwSize);
	ULONGLONG ullVA = rVA2VA(ullRVA, lpFileImage, dwSize);
	wchar_t szOffset[MAX_PATH] = { 0 };
	wchar_t szVA[MAX_PATH] = { 0 };
	_ui64tow_s(ullOffset, szOffset, MAX_PATH, 16);
	_ui64tow_s(ullVA, szVA, MAX_PATH, 16);
	this->SetDlgItemTextW(IDC_EDIT1, szVA);
	this->SetDlgItemTextW(IDC_EDIT3, szOffset);
	// TODO:  Add your control notification handler code here
}


ULONGLONG CCalc::RVA2Offset(ULONGLONG rVA, PVOID lpImage, DWORD dwSize)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((size_t)lpImage + pDos->e_lfanew);

	PIMAGE_FILE_HEADER pFileHeader = &(pNT->FileHeader);

	for (WORD i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNT);
		ULONGLONG rVAbegin = pSection[i].VirtualAddress;
		ULONGLONG rVAend = pSection[i].Misc.VirtualSize + pSection[i].VirtualAddress;
		if (rVA >= rVAbegin && rVA < rVAend)
		{
			return rVA - pSection[i].VirtualAddress + pSection[i].PointerToRawData;
		}

	}
	return -1;
}

ULONGLONG CCalc::Offset2rVA(ULONGLONG offset, PVOID lpImage, DWORD dwSize)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((size_t)lpImage + pDos->e_lfanew);

	PIMAGE_FILE_HEADER pFileHeader = &(pNT->FileHeader);

	for (WORD i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNT);
		ULONGLONG dwSecBegin = pSection[i].PointerToRawData;
		ULONGLONG dwSecEnd = pSection[i].PointerToRawData + pSection[i].SizeOfRawData;
		if (offset >= dwSecBegin && offset < dwSecEnd)
		{
			return offset - pSection[i].PointerToRawData + pSection[i].VirtualAddress;
		}
	}
	return -1;
}

ULONGLONG CCalc::rVA2VA(ULONGLONG rVA, PVOID lpImage, DWORD dwSize)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((size_t)lpImage + pDos->e_lfanew);

	if (isx64)
	{
		PIMAGE_OPTIONAL_HEADER64 pOptional = (PIMAGE_OPTIONAL_HEADER64)&(pNT->OptionalHeader);
		return pOptional->ImageBase + rVA;
	}
	PIMAGE_OPTIONAL_HEADER pOptional = &(pNT->OptionalHeader);
	return pOptional->ImageBase + rVA;
}

ULONGLONG CCalc::VA2rVA(ULONGLONG VA, PVOID lpImage, DWORD dwSize)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((size_t)lpImage + pDos->e_lfanew);

	if (isx64)
	{
		PIMAGE_OPTIONAL_HEADER64 pOptional = (PIMAGE_OPTIONAL_HEADER64)&(pNT->OptionalHeader);
		if (VA < pOptional->ImageBase)
		{
			return -1;
		}
		else
		{
			return VA - pOptional->ImageBase;
		}
	}

	PIMAGE_OPTIONAL_HEADER pOptional = &(pNT->OptionalHeader);

	if (VA < pOptional->ImageBase)
	{
		return -1;
	}
	else
	{
		return VA - pOptional->ImageBase;
	}
}

void CCalc::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString szVA;
	wchar_t *endPoint;
	CEdit* edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT1);
	if (!edit_value->IsWindowEnabled())
	{
		return;
	}
	edit_value->GetWindowTextW(szVA);
	ULONGLONG ullVA = wcstol(szVA, &endPoint, 16);
	if (ullVA == 0)
	{
		return;
	}
	ULONGLONG ullRVA = VA2rVA(ullVA, lpFileImage, dwSize);
	ULONGLONG ullOffset = RVA2Offset(ullRVA, lpFileImage, dwSize);
	wchar_t szOffset[MAX_PATH] = { 0 };
	wchar_t szRVA[MAX_PATH] = { 0 };
	_ui64tow_s(ullOffset, szOffset, MAX_PATH, 16);
	_ui64tow_s(ullRVA, szRVA, MAX_PATH, 16);
	this->SetDlgItemTextW(IDC_EDIT2, szRVA);
	this->SetDlgItemTextW(IDC_EDIT3, szOffset);
	// TODO:  Add your control notification handler code here
}




void CCalc::OnEnChangeEdit3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	CString szOffset;
	wchar_t *endPoint;
	CEdit* edit_value = (CEdit*)this->GetDlgItem(IDC_EDIT3);
	if (!edit_value->IsWindowEnabled())
	{
		return;
	}
	edit_value->GetWindowTextW(szOffset);
	ULONGLONG ullOffset = wcstol(szOffset, &endPoint, 16);
	if (ullOffset == 0)
	{
		return;
	}
	ULONGLONG ullRVA = Offset2rVA(ullOffset, lpFileImage, dwSize);
	ULONGLONG ullVA = rVA2VA(ullRVA, lpFileImage, dwSize);
	wchar_t szRVA[MAX_PATH] = { 0 };
	wchar_t szVA[MAX_PATH] = { 0 };
	_ui64tow_s(ullRVA, szRVA, MAX_PATH, 16);
	_ui64tow_s(ullVA, szVA, MAX_PATH, 16);
	this->SetDlgItemTextW(IDC_EDIT1, szVA);
	this->SetDlgItemTextW(IDC_EDIT2, szRVA);
	// TODO:  Add your control notification handler code here
}


BOOL CCalc::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	OnBnClickedButton2();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
