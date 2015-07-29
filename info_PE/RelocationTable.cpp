// RelocationTable.cpp : implementation file
//

#include "stdafx.h"
#include "info_PE.h"
#include "RelocationTable.h"
#include "afxdialogex.h"
#include <strsafe.h>

// CRelocationTable dialog

IMPLEMENT_DYNAMIC(CRelocationTable, CDialogEx)

CRelocationTable::CRelocationTable(CWnd* pParent /*=NULL*/)
: CDialogEx(CRelocationTable::IDD, pParent)
{

}

CRelocationTable::~CRelocationTable()
{
}

void CRelocationTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST8, m_blockTable);
	DDX_Control(pDX, IDC_LIST9, m_itemTable);
}


BEGIN_MESSAGE_MAP(CRelocationTable, CDialogEx)
	ON_NOTIFY(NM_CLICK, IDC_LIST8, &CRelocationTable::OnNMClickList8)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST8, &CRelocationTable::OnLvnKeydownList8)
END_MESSAGE_MAP()


// CRelocationTable message handlers


BOOL CRelocationTable::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_blockTable.AddColumn(4, L"序号", 130, L"区段", 130, L"rVA", 130, L"个数", 130);
	m_itemTable.AddColumn(5, L"索引", 100, L"rVA", 100, L"Offset", 100, L"Type", 110, L"Far Address", 100);
	getRelocTable(lpFileImage, dwSize);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CRelocationTable::getRelocTable(PVOID lpBuf, DWORD dwSize)
{
	m_blockTable.DeleteAllItems();
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

	PIMAGE_DATA_DIRECTORY pRelocDir = &pDir[IMAGE_DIRECTORY_ENTRY_BASERELOC];

	ULONGLONG stRelocOffset = RVA2Offset(pRelocDir->VirtualAddress, lpFileImage, dwSize);

	PIMAGE_BASE_RELOCATION pRelocBase = (PIMAGE_BASE_RELOCATION)(stRelocOffset + (size_t)lpBuf);

	while (pRelocBase->VirtualAddress)
	{
		wchar_t szIndex[MAX_PATH] = { 0 };
		wchar_t szRVA[MAX_PATH] = { 0 };
		wchar_t szSection[MAX_PATH] = { 0 };
		wchar_t szCount[MAX_PATH] = { 0 };
		int nIndex = m_blockTable.GetItemCount();
		wsprintf(szIndex, L"%#08X", nIndex + 1);
		wsprintf(szRVA, L"%#08X", pRelocBase->VirtualAddress);
		StringCchCopy(szSection, MAX_PATH, getSection(pRelocBase->VirtualAddress, lpFileImage, dwSize));
		wsprintf(szCount, L"%#08X", ((pRelocBase->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(TYPE_OFFSET)));
		m_blockTable.Additem(nIndex, 4, szIndex, szSection, szRVA, szCount);

		pRelocBase = (PIMAGE_BASE_RELOCATION)((size_t)pRelocBase + pRelocBase->SizeOfBlock);

	}
}

ULONGLONG CRelocationTable::RVA2Offset(ULONGLONG rVA, PVOID lpImage, DWORD dwSize)
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

wchar_t* CRelocationTable::getSection(ULONGLONG rVA, PVOID lpImage, DWORD dwSize)
{
	static wchar_t szSection[MAX_PATH] = { 0 };
	char szSectionName[32] = { 0 };
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
			sprintf_s(szSectionName, "%s", pSection[i].Name);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSectionName, -1, szSection, MAX_PATH);
			return szSection;
		}

	}
	return szSection;
}

void CRelocationTable::OnNMClickList8(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	POSITION pos = m_blockTable.GetFirstSelectedItemPosition();

	int nIndex = -1;
	nIndex = m_blockTable.GetNextSelectedItem(pos);

	wchar_t szCount[MAX_PATH] = { 0 };
	wchar_t szRVA[MAX_PATH] = { 0 };
	wchar_t *endPtr;

	m_blockTable.GetItemText(nIndex, 2, szRVA, MAX_PATH);
	m_blockTable.GetItemText(nIndex, 3, szCount, MAX_PATH);

	size_t dwCount = wcstol(szCount, &endPtr, 16);
	size_t dwRVA = wcstol(szRVA, &endPtr, 16);

	getRelocItems(dwRVA, dwCount, lpFileImage, dwSize);

	*pResult = 0;
}

void CRelocationTable::getRelocItems(ULONGLONG ullRVA, DWORD dwCount, PVOID lpBuf, DWORD dwSize)
{
	m_itemTable.DeleteAllItems();

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
	PIMAGE_DATA_DIRECTORY pRelocDir = &pDir[IMAGE_DIRECTORY_ENTRY_BASERELOC];

	ULONGLONG ullRelocOffset = RVA2Offset(pRelocDir->VirtualAddress, lpFileImage, dwSize);

	PIMAGE_BASE_RELOCATION pRelocBase = (PIMAGE_BASE_RELOCATION)(ullRelocOffset + (size_t)lpBuf);

	while (pRelocBase->VirtualAddress != ullRVA)
	{
		pRelocBase = (PIMAGE_BASE_RELOCATION)((size_t)pRelocBase + pRelocBase->SizeOfBlock);
	}

	ULONGLONG ullBaseOffset = RVA2Offset(ullRVA, lpFileImage, dwSize);
	wchar_t szIndex[MAX_PATH] = { 0 };
	wchar_t szRVA[MAX_PATH] = { 0 };
	wchar_t szOffset[MAX_PATH] = { 0 };
	wchar_t szType[MAX_PATH] = { 0 };
	wchar_t szFarAddr[MAX_PATH] = { 0 };


	PTYPE_OFFSET pType = (PTYPE_OFFSET)(pRelocBase + 1);
	if (isx64)
	{
		for (DWORD i = 0; i < dwCount; i++)
		{
			PDWORD pdwAddr = nullptr;
			wsprintf(szIndex, L"%#08X", i);
			wsprintf(szRVA, L"%#08X", pType[i].Offset + ullRVA);
			wsprintf(szOffset, L"%#08X", pType[i].Offset + ullBaseOffset);
			StringCchCopy(szType, MAX_PATH, getRelocType(pType[i].Type));
			pdwAddr = (PDWORD)(pType[i].Offset + ullBaseOffset + (size_t)lpFileImage);
			wsprintf(szFarAddr, L"%#08X", *pdwAddr);
			m_itemTable.Additem(i, 5, szIndex, szRVA, szOffset, szType, szFarAddr);
		}
	}
	else
	{
		for (DWORD i = 0; i < dwCount; i++)
		{
			PDWORD pdwAddr = nullptr;
			wsprintf(szIndex, L"%#08X", i);
			wsprintf(szRVA, L"%#08X", pType[i].Offset + ullRVA);
			wsprintf(szOffset, L"%#08X", pType[i].Offset + ullBaseOffset);
			StringCchCopy(szType, MAX_PATH, getRelocType(pType[i].Type));
			pdwAddr = (PDWORD)(pType[i].Offset + ullBaseOffset + (size_t)lpFileImage);
			wsprintf(szFarAddr, L"%#08X", *pdwAddr);
			m_itemTable.Additem(i, 5, szIndex, szRVA, szOffset, szType, szFarAddr);
		}
	}
}

wchar_t* CRelocationTable::getRelocType(DWORD dwType)
{
	static wchar_t szType[MAX_PATH] = { 0 };
	switch (dwType)
	{
	case 0x00:
	{
				 StringCchCopy(szType, MAX_PATH, L"无重定位[0x00]");
				 break;
	}
	case 0x01:
	{
				 StringCchCopy(szType, MAX_PATH, L"高2字节[0x01]");
				 break;
	}
	case 0x02:
	{
				 StringCchCopy(szType, MAX_PATH, L"低2字节[0x02]");
				 break;
	}
	case 0x03:
	{
				 StringCchCopy(szType, MAX_PATH, L"整4字节[0x03]");
				 break;
	}
	case 0x04:
	{
				 StringCchCopy(szType, MAX_PATH, L"两项合并[0x04]");
				 break;
	}
	case 0x05:
	{
				 StringCchCopy(szType, MAX_PATH, L"用于MISP JUMP[0x05]");
				 break;
	}
	case 0x06:
	{
				 StringCchCopy(szType, MAX_PATH, L"保留[0x06]");
				 break;
	}
	case 0x09:
	{
				 StringCchCopy(szType, MAX_PATH, L"用于MISP16 JUMP[0x09]");
				 break;
	}
	case 0x0a:
	{
				 StringCchCopy(szType, MAX_PATH, L"64位修正[0x0A]");
				 break;
	}
	default:
	{
			   wsprintf(szType, L"未定义[%#08X]", dwType);
			   break;
	}

	}
	return szType;
}

void CRelocationTable::OnLvnKeydownList8(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	POSITION pos = m_blockTable.GetFirstSelectedItemPosition();

	int nIndex = -1;
	nIndex = m_blockTable.GetNextSelectedItem(pos);

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
					if (nIndex < m_blockTable.GetItemCount() - 1)
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

	wchar_t szRVA[MAX_PATH] = { 0 };
	wchar_t szCount[MAX_PATH] = { 0 };
	wchar_t *endPtr;

	m_blockTable.GetItemText(nIndex, 2, szRVA, MAX_PATH);
	m_blockTable.GetItemText(nIndex, 3, szCount, MAX_PATH);

	size_t dwCount = wcstol(szCount, &endPtr, 16);
	ULONGLONG ullRVA = wcstol(szRVA, &endPtr, 16);

	getRelocItems(ullRVA, dwCount, lpFileImage, dwSize);

	*pResult = 0;
}
