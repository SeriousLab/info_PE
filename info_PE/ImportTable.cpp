// ImportTable.cpp : implementation file
//

#include "stdafx.h"
#include "info_PE.h"
#include "ImportTable.h"
#include "afxdialogex.h"


// CImportTable dialog

IMPLEMENT_DYNAMIC(CImportTable, CDialogEx)

CImportTable::CImportTable(CWnd* pParent /*=NULL*/)
: CDialogEx(CImportTable::IDD, pParent)
{

}

CImportTable::~CImportTable()
{
}

void CImportTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST5, m_importDLL);
	DDX_Control(pDX, IDC_LIST6, m_importFunctions);
}


BEGIN_MESSAGE_MAP(CImportTable, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST5, &CImportTable::OnNMClickList5)
	//	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST5, &CImportTable::OnLvnKeydownList5)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST5, &CImportTable::OnLvnKeydownList5)
END_MESSAGE_MAP()


// CImportTable message handlers
void CImportTable::printImportDLL(PVOID lpBuf, DWORD dwSize)
{
	int DLLCounter = 0;

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpBuf;
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((size_t)lpBuf + pDos->e_lfanew);

	PIMAGE_DATA_DIRECTORY pDir = nullptr;

	if (isx64)
	{
		pDir = (PIMAGE_DATA_DIRECTORY)((size_t)&(pNT->OptionalHeader) + sizeof(DWORD)* 28);
	}
	else
	{
		pDir = pNT->OptionalHeader.DataDirectory;
	}

	PIMAGE_DATA_DIRECTORY pImportDir = &pDir[IMAGE_DIRECTORY_ENTRY_IMPORT];

	if (pDir[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0 || pDir[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0)
	{
		MessageBox(L"无导入表!");
		return;
	}

	ULONGLONG ullImportOffset = RVA2Offset(pImportDir->VirtualAddress, lpBuf, dwSize);
	PIMAGE_IMPORT_DESCRIPTOR pImport = (PIMAGE_IMPORT_DESCRIPTOR)(ullImportOffset + (size_t)lpBuf);

	while (pImport->Name)
	{
		ULONGLONG ullNameOffset = RVA2Offset(pImport->Name, lpBuf, dwSize);
		PCHAR pstrName = (PCHAR)(ullNameOffset + (size_t)lpBuf);
		ULONGLONG ullIATOffset = RVA2Offset(pImport->FirstThunk, lpBuf, dwSize);
		ULONGLONG ullINTOffset = RVA2Offset(pImport->OriginalFirstThunk, lpBuf, dwSize);

		wchar_t szName[MAX_PATH] = { 0 };
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pstrName, -1, szName, MAX_PATH);
		wchar_t szIAT[MAX_PATH] = { 0 };
		wchar_t szINT[MAX_PATH] = { 0 };
		wsprintf(szIAT, L"%#08X", ullIATOffset);
		wsprintf(szINT, L"%#08X", ullINTOffset);
		m_importDLL.Additem(DLLCounter, 3, szName, szIAT, szINT);
		pImport++;
		DLLCounter++;
	}
}

ULONGLONG CImportTable::RVA2Offset(ULONGLONG rVA, PVOID lpImage, DWORD dwSize)
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

BOOL CImportTable::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_importDLL.AddColumn(3, L"模块名称", 300, L"IAT文件偏移", 150, L"INT文件偏移", 150);
	m_importFunctions.AddColumn(4, L"索引", 100, L"函数名", 200, L"rVA", 150, L"偏移", 150);

	printImportDLL(lpFileImage, dwSize);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CImportTable::OnNMClickList5(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION pos = m_importDLL.GetFirstSelectedItemPosition();

	int nIndex = -1;
	nIndex = m_importDLL.GetNextSelectedItem(pos);

	wchar_t szIAT[MAX_PATH] = { 0 };
	wchar_t szINT[MAX_PATH] = { 0 };
	wchar_t *endPtr;

	m_importDLL.GetItemText(nIndex, 1, szIAT, MAX_PATH);
	m_importDLL.GetItemText(nIndex, 2, szINT, MAX_PATH);

	ULONGLONG ullIAT, ullINT;
	ullIAT = wcstol(szIAT, &endPtr, 16);
	ullINT = wcstol(szINT, &endPtr, 16);

	if (ullIAT == 0 || ullINT == 0)
	{
		m_importFunctions.DeleteAllItems();
		return;
	}
	if (ullIAT == 0x7fffffff && ullINT == 0x7fffffff)
	{
		MessageBox(L"IAT与INT地址无效!");
		return;
	}
	if (ullINT == 0x7fffffff)
	{
		printImportFunctions(ullIAT, ullINT, TRUE);
	}
	else
	{
		printImportFunctions(ullIAT, ullINT, FALSE);
	}
	*pResult = 0;
}


void CImportTable::printImportFunctions(ULONGLONG iAT, ULONGLONG iNT, BOOL isIAT)
{
	m_importFunctions.DeleteAllItems();

	int nIndex = 0;

	if (!isx64)
	{
		PIMAGE_THUNK_DATA pIAT = (PIMAGE_THUNK_DATA)(iAT + (size_t)lpFileImage);
		PIMAGE_THUNK_DATA pINT = (PIMAGE_THUNK_DATA)(iNT + (size_t)lpFileImage);

		PIMAGE_THUNK_DATA pFunctions = nullptr;
		if (isIAT)
		{
			pFunctions = pIAT;
		}
		else
		{
			pFunctions = pINT;
		}
		while (pFunctions->u1.AddressOfData)
		{
			if (IMAGE_SNAP_BY_ORDINAL32(pFunctions->u1.AddressOfData))
			{
				wchar_t szRVA[MAX_PATH] = { 0 };
				wchar_t szOffset[MAX_PATH] = { 0 };
				wchar_t szOrdinal[MAX_PATH] = { 0 };
				wsprintf(szRVA, L"%#08X", pFunctions->u1.AddressOfData);
				ULONGLONG ullOffset = RVA2Offset(pFunctions->u1.AddressOfData, lpFileImage, dwSize);
				wsprintf(szOffset, L"%#08X", ullOffset);
				wsprintf(szOrdinal, L"%#04X", pFunctions->u1.Ordinal & 0x0000ffff);
				m_importFunctions.Additem(nIndex, 4, szOrdinal, L"-", szRVA, szOffset);
			}
			else
			{
				wchar_t szRVA[MAX_PATH] = { 0 };
				wchar_t szOffset[MAX_PATH] = { 0 };
				wchar_t szOrdinal[MAX_PATH] = { 0 };
				wchar_t szName[MAX_PATH] = { 0 };
				ULONGLONG ullByNameOffset = RVA2Offset(pFunctions->u1.AddressOfData, lpFileImage, dwSize);
				PIMAGE_IMPORT_BY_NAME pByName = (PIMAGE_IMPORT_BY_NAME)(ullByNameOffset + (size_t)lpFileImage);
				wsprintf(szRVA, L"%#08X", pFunctions->u1.AddressOfData);
				ULONGLONG ullOffset = RVA2Offset(pFunctions->u1.AddressOfData, lpFileImage, dwSize);
				wsprintf(szOffset, L"%#08X", ullOffset);
				wsprintf(szOrdinal, L"%#04X", pByName->Hint);
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pByName->Name, -1, szName, MAX_PATH);
				m_importFunctions.Additem(nIndex, 4, szOrdinal, szName, szRVA, szOffset);
			}
			pFunctions++;
			nIndex++;
		}
	}
	else
	{
		PIMAGE_THUNK_DATA64 pIAT = (PIMAGE_THUNK_DATA64)(iAT + (size_t)lpFileImage);
		PIMAGE_THUNK_DATA64 pINT = (PIMAGE_THUNK_DATA64)(iNT + (size_t)lpFileImage);

		PIMAGE_THUNK_DATA64 pFunctions = nullptr;
		if (isIAT)
		{
			pFunctions = pIAT;
		}
		else
		{
			pFunctions = pINT;
		}
		while (pFunctions->u1.AddressOfData)
		{
			if (IMAGE_SNAP_BY_ORDINAL64(pFunctions->u1.AddressOfData))
			{
				wchar_t szRVA[MAX_PATH] = { 0 };
				wchar_t szOffset[MAX_PATH] = { 0 };
				wchar_t szOrdinal[MAX_PATH] = { 0 };
				wsprintf(szRVA, L"%#016X", pFunctions->u1.AddressOfData);
				ULONGLONG ullOffset = RVA2Offset(pFunctions->u1.AddressOfData, lpFileImage, dwSize);
				wsprintf(szOffset, L"%#016X", ullOffset);
				wsprintf(szOrdinal, L"%#04X", pFunctions->u1.Ordinal & 0x0000ffff);
				m_importFunctions.Additem(nIndex, 4, szOrdinal, L"-", szRVA, szOffset);
			}
			else
			{
				wchar_t szRVA[MAX_PATH] = { 0 };
				wchar_t szOffset[MAX_PATH] = { 0 };
				wchar_t szOrdinal[MAX_PATH] = { 0 };
				wchar_t szName[MAX_PATH] = { 0 };
				ULONGLONG ullByNameOffset = RVA2Offset(pFunctions->u1.AddressOfData, lpFileImage, dwSize);
				PIMAGE_IMPORT_BY_NAME pByName = (PIMAGE_IMPORT_BY_NAME)(ullByNameOffset + (size_t)lpFileImage);
				wsprintf(szRVA, L"%#016X", pFunctions->u1.AddressOfData);
				ULONGLONG ullOffset = RVA2Offset(pFunctions->u1.AddressOfData, lpFileImage, dwSize);
				wsprintf(szOffset, L"%#016X", ullOffset);
				wsprintf(szOrdinal, L"%#04X", pByName->Hint);
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pByName->Name, -1, szName, MAX_PATH);
				m_importFunctions.Additem(nIndex, 4, szOrdinal, szName, szRVA, szOffset);
			}
			pFunctions++;
			nIndex++;
		}
	}
}



void CImportTable::OnLvnKeydownList5(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION pos = m_importDLL.GetFirstSelectedItemPosition();

	int nIndex = -1;
	nIndex = m_importDLL.GetNextSelectedItem(pos);

	switch (pLVKeyDow->wVKey)
	{
	case VK_UP:
	{
				  if (nIndex != 0)
				  {
					  nIndex -= 1;
				  }
				  else
				  {
					  return;
				  }
				  break;
	}
	case VK_DOWN:
	{
					if (nIndex < m_importDLL.GetItemCount() - 1)
					{
						nIndex += 1;
					}
					else
					{
						return;
					}
					break;
	}
	default:
		return;
	}

	wchar_t szIAT[MAX_PATH] = { 0 };
	wchar_t szINT[MAX_PATH] = { 0 };
	wchar_t *endPtr;

	m_importDLL.GetItemText(nIndex, 1, szIAT, MAX_PATH);
	m_importDLL.GetItemText(nIndex, 2, szINT, MAX_PATH);

	ULONGLONG ullIAT, ullINT;
	ullIAT = wcstol(szIAT, &endPtr, 16);
	ullINT = wcstol(szINT, &endPtr, 16);

	if (ullIAT == 0 || ullINT == 0)
	{
		m_importFunctions.DeleteAllItems();
		return;
	}
	if (ullIAT == 0x7fffffff && ullINT == 0x7fffffff)
	{
		MessageBox(L"IAT与INT地址无效!");
		return;
	}
	if (ullINT == 0x7fffffff)
	{
		printImportFunctions(ullIAT, ullINT, TRUE);
	}
	else
	{
		printImportFunctions(ullIAT, ullINT, FALSE);
	}
	*pResult = 0;
}
