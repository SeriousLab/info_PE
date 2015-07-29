// ResourceTable.cpp : implementation file
//

#include "stdafx.h"
#include "info_PE.h"
#include "ResourceTable.h"
#include "afxdialogex.h"
#include <strsafe.h>

// CResourceTable dialog

IMPLEMENT_DYNAMIC(CResourceTable, CDialogEx)

CResourceTable::CResourceTable(CWnd* pParent /*=NULL*/)
: CDialogEx(CResourceTable::IDD, pParent)
{

}

CResourceTable::~CResourceTable()
{
}

void CResourceTable::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_ResourceTree);
	DDX_Control(pDX, IDC_LIST7, m_ResourceItems);
}


BEGIN_MESSAGE_MAP(CResourceTable, CDialogEx)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CResourceTable::OnTvnSelchangedTree1)
END_MESSAGE_MAP()


// CResourceTable message handlers


BOOL CResourceTable::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	HTREEITEM hRoot;
	hRoot = m_ResourceTree.InsertItem(L"\\");
	m_ResourceItems.AddColumn(3, L"名称", 100, L"命名资源数", 100, L"ID资源数", 100);

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpFileImage;
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((size_t)lpFileImage + pDos->e_lfanew);

	PIMAGE_DATA_DIRECTORY pDir = nullptr;

	if (isx64)
	{
		pDir = (PIMAGE_DATA_DIRECTORY)((size_t)&(pNT->OptionalHeader) + sizeof(DWORD)* 28);
	}
	else
	{
		pDir = pNT->OptionalHeader.DataDirectory;
	}
	PIMAGE_DATA_DIRECTORY pResDir = &pDir[IMAGE_DIRECTORY_ENTRY_RESOURCE];

	ULONGLONG ullResOffset = RVA2Offset(pResDir->VirtualAddress, lpFileImage, dwSize);

	pBase = (PIMAGE_RESOURCE_DIRECTORY)(ullResOffset + (size_t)lpFileImage);

	PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pBase + 1);

	getResourceTable(hRoot, pBase, pEntry, FALSE, pBase->NumberOfNamedEntries + pBase->NumberOfIdEntries, TRUE);

	m_ResourceTree.Expand(hRoot, TVE_EXPAND);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CResourceTable::getResourceTable(HTREEITEM hNode, PIMAGE_RESOURCE_DIRECTORY pRes, PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry, BOOL isList, DWORD dwCount, BOOL isRoot)
{
	if (isList)
	{
		wchar_t szName[MAX_PATH] = { 0 };
		wchar_t szCountID[MAX_PATH] = { 0 };
		wchar_t szCountName[MAX_PATH] = { 0 };
		for (DWORD q = 0; q < dwCount; q++)
		{
			if (!pEntry[q].DataIsDirectory)
			{
				CEdit* editValue = nullptr;
				m_ResourceItems.DeleteAllItems();
				PIMAGE_RESOURCE_DATA_ENTRY pResData = (PIMAGE_RESOURCE_DATA_ENTRY)(pEntry[q].OffsetToData + (size_t)pBase);
				wchar_t szResOffset[MAX_PATH] = { 0 };
				wchar_t szResSize[MAX_PATH] = { 0 };
				wchar_t szResRVA[MAX_PATH] = { 0 };
				wsprintf(szResRVA, L"%#08X", pResData->OffsetToData);
				wsprintf(szResSize, L"%#08X", pResData->Size);
				ULONGLONG ullOffset = RVA2Offset(pResData->OffsetToData, lpFileImage, dwSize);
				wsprintf(szResOffset, L"%#08X", ullOffset);
				editValue = (CEdit*)this->GetDlgItem(Edit_Res_Offset);
				editValue->SetWindowTextW(szResOffset);
				editValue = (CEdit*)this->GetDlgItem(Edit_Res_rVA);
				editValue->SetWindowTextW(szResRVA);
				editValue = (CEdit*)this->GetDlgItem(Edit_Res_Size);
				editValue->SetWindowTextW(szResSize);
				continue;
			}
			else
			{
				if (pEntry[q].NameIsString)
				{
					PIMAGE_RESOURCE_DIR_STRING_U pstrID = (PIMAGE_RESOURCE_DIR_STRING_U)(pEntry[q].NameOffset + (size_t)pBase);
					StringCchCopy(szName, pstrID->Length + 1, pstrID->NameString);
				}
				else
				{
					if (isRoot)
					{
						StringCchCopy(szName, MAX_PATH, getTypeName(pEntry[q].Id));
					}
					else
					{
						wsprintf(szName, L"%4d", pEntry[q].Id);
					}
				}
				PIMAGE_RESOURCE_DIRECTORY pNext = (PIMAGE_RESOURCE_DIRECTORY)(pEntry[q].OffsetToDirectory + (size_t)pBase);
				wsprintf(szCountName, L"%#08X", pNext->NumberOfNamedEntries);
				wsprintf(szCountID, L"%#08X", pNext->NumberOfIdEntries);
				m_ResourceItems.Additem(q, 3, szName, szCountName, szCountID);
			}
		}
	}

	else
	{
		HTREEITEM hCurrent = nullptr;
		for (DWORD i = 0; i < dwCount; i++)
		{
			if (pEntry[i].NameIsString)
			{
				PIMAGE_RESOURCE_DIR_STRING_U pstrID = (PIMAGE_RESOURCE_DIR_STRING_U)(pEntry[i].NameOffset + (size_t)pBase);
				wchar_t szEntryName[MAX_PATH] = { 0 };
				StringCchCopy(szEntryName, pstrID->Length + 1, pstrID->NameString);
				hCurrent = m_ResourceTree.InsertItem(szEntryName, hNode);
			}
			else
			{
				wchar_t szID[MAX_PATH] = { 0 };
				if (isRoot)
				{
					StringCchCopy(szID, MAX_PATH, getTypeName(pEntry[i].Id));
				}
				else
				{
					wsprintf(szID, L"%4d", pEntry[i].Id);
				}

				hCurrent = m_ResourceTree.InsertItem(szID, hNode);
			}
			if (pEntry[i].DataIsDirectory)
			{
				m_ResourceTree.SetItemData(hCurrent, pEntry[i].OffsetToDirectory);
				PIMAGE_RESOURCE_DIRECTORY pNext = (PIMAGE_RESOURCE_DIRECTORY)(pEntry[i].OffsetToDirectory + (size_t)pBase);
				PIMAGE_RESOURCE_DIRECTORY_ENTRY pNextEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pNext + 1);
				WORD wNum = pNext->NumberOfIdEntries + pNext->NumberOfNamedEntries;
				getResourceTable(hCurrent, pNext, pNextEntry, FALSE, wNum, FALSE);
			}
			else
			{
				m_ResourceTree.SetItemData(hCurrent, pEntry[i].OffsetToData);
			}
		}
	}
}

ULONGLONG CResourceTable::RVA2Offset(ULONGLONG rVA, PVOID lpImage, DWORD dwSize)
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

wchar_t* CResourceTable::getTypeName(DWORD dwID)
{
	static wchar_t szTypeName[MAX_PATH] = { 0 };
	switch (dwID)
	{
	case 0x01:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"鼠标指针(Cursor)");
				 break;
	}
	case 0x02:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"位图(Bitmap)");
				 break;
	}
	case 0x03:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"图标(Icon)");
				 break;
	}
	case 0x04:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"菜单(Menu)");
				 break;
	}
	case 0x05:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"对话框(Dialog)");
				 break;
	}
	case 0x06:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"字符串列表(String table)");
				 break;
	}
	case 0x07:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"字体目录(Font directory)");
				 break;
	}
	case 0x08:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"字体(Font)");
				 break;
	}
	case 0x09:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"快捷键(Accelerators)");
				 break;
	}
	case 0x0a:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"非格式化资源(Unformatted)");
				 break;
	}
	case 0x0b:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"消息列表(Message table)");
				 break;
	}
	case 0x0c:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"鼠标指针组(Group cursor)");
				 break;
	}
	case 0x0e:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"图标组(Group icon)");
				 break;
	}
	case 0x10:
	{
				 StringCchCopy(szTypeName, MAX_PATH, L"版本信息(Version information)");
				 break;
	}
	default:
	{
			   wsprintf(szTypeName, L"%04d", dwID);
			   break;
	}

	}
	return szTypeName;
}

void CResourceTable::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSel = m_ResourceTree.GetSelectedItem();

	CEdit* editValue = nullptr;

	if (hSel == NULL)
	{
		return;
	}
	if (m_ResourceTree.ItemHasChildren(hSel))
	{
		m_ResourceItems.DeleteAllItems();
		editValue = (CEdit*)this->GetDlgItem(Edit_Res_Offset);
		editValue->SetWindowTextW(L"");
		editValue = (CEdit*)this->GetDlgItem(Edit_Res_Size);
		editValue->SetWindowTextW(L"");
		editValue = (CEdit*)this->GetDlgItem(Edit_Res_rVA);
		editValue->SetWindowTextW(L"");
		CString szRoot = m_ResourceTree.GetItemText(hSel);
		PIMAGE_RESOURCE_DIRECTORY pRoot = (PIMAGE_RESOURCE_DIRECTORY)(m_ResourceTree.GetItemData(hSel) + (size_t)pBase);
		PIMAGE_RESOURCE_DIRECTORY_ENTRY pNextEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pRoot + 1);
		WORD wNum = pRoot->NumberOfIdEntries + pRoot->NumberOfNamedEntries;
		if (szRoot == L"\\")
		{
			getResourceTable(hSel, pRoot, pNextEntry, TRUE, wNum, TRUE);
		}
		else
		{
			getResourceTable(hSel, pRoot, pNextEntry, TRUE, wNum, FALSE);
		}
	}
	else
	{
		m_ResourceItems.DeleteAllItems();
		PIMAGE_RESOURCE_DATA_ENTRY pResData = (PIMAGE_RESOURCE_DATA_ENTRY)(m_ResourceTree.GetItemData(hSel) + (size_t)pBase);
		wchar_t szResOffset[MAX_PATH] = { 0 };
		wchar_t szResSize[MAX_PATH] = { 0 };
		wchar_t szResRVA[MAX_PATH] = { 0 };
		wsprintf(szResRVA, L"%#08X", pResData->OffsetToData);
		wsprintf(szResSize, L"%#08X", pResData->Size);
		ULONGLONG ullOffset = RVA2Offset(pResData->OffsetToData, lpFileImage, dwSize);
		wsprintf(szResOffset, L"%#08X", ullOffset);
		editValue = (CEdit*)this->GetDlgItem(Edit_Res_Offset);
		editValue->SetWindowTextW(szResOffset);
		editValue = (CEdit*)this->GetDlgItem(Edit_Res_rVA);
		editValue->SetWindowTextW(szResRVA);
		editValue = (CEdit*)this->GetDlgItem(Edit_Res_Size);
		editValue->SetWindowTextW(szResSize);
	}

	*pResult = 0;
}
