
// info_PEDlg.cpp : implementation file
//

#include "stdafx.h"
#include "info_PE.h"
#include "info_PEDlg.h"
#include "afxdialogex.h"
#include <strsafe.h>
#include "Calc.h"
#include "ExportTable.h"
#include "ImportTable.h"
#include "ResourceTable.h"
#include "RelocationTable.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cinfo_PEDlg dialog



Cinfo_PEDlg::Cinfo_PEDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(Cinfo_PEDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cinfo_PEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ExtendedHeader);
	DDX_Control(pDX, IDC_LIST2, m_DataDir);
	DDX_Control(pDX, IDC_LIST3, m_SectionInfo);
}

BEGIN_MESSAGE_MAP(Cinfo_PEDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_CALC_32771, &Cinfo_PEDlg::OnCalc32771)
	ON_COMMAND(ID_CALC_32772, &Cinfo_PEDlg::OnCalc32772)
	ON_COMMAND(ID_CALC_32773, &Cinfo_PEDlg::OnCalc32773)
	ON_COMMAND(ID_CALC_32774, &Cinfo_PEDlg::OnCalc32774)
	ON_COMMAND(ID_CALC_32775, &Cinfo_PEDlg::OnCalc32775)
	ON_COMMAND(ID_CALC_32776, &Cinfo_PEDlg::OnCalc32776)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// Cinfo_PEDlg message handlers

BOOL Cinfo_PEDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	DragAcceptFiles(TRUE);
	// TODO: Add extra initialization here

	m_ExtendedHeader.AddColumn(2, L"Name", 180, L"Value", 200);
	m_DataDir.AddColumn(3, L"Name", 140, L"Addr", 90, L"Size", 90);
	m_SectionInfo.AddColumn(5, L"Name", 50, L"Offset", 80, L"Size", 80, L"rVA", 80, L"Size", 80);

	lpFileImage = nullptr;
	dwSize = 0;



	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cinfo_PEDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cinfo_PEDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cinfo_PEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cinfo_PEDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	wchar_t szDropFile[MAX_PATH];
	DragQueryFile(hDropInfo, 0, szDropFile, 260);
	StringCchCopy(szFile, MAX_PATH, szDropFile);
	getPEinfo(szDropFile);

	CDialogEx::OnDropFiles(hDropInfo);
}


void Cinfo_PEDlg::getPEinfo(LPCTSTR szFileName)
{
	if (lpFileImage != nullptr)
	{
		VirtualFree(lpFileImage, 0, MEM_RELEASE);
		lpFileImage = nullptr;
	}

	dwSize = 0;


	if (INVALID_HANDLE_VALUE == (hFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
	{
		MessageBox(L"打开文件失败!");
		return;
	}
	if (INVALID_FILE_SIZE == (dwSize = GetFileSize(hFile, NULL)))
	{
		CloseHandle(hFile);
		MessageBox(L"获取文件大小失败!");
		return;
	}
	if (!(lpFileImage = VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE)))
	{
		CloseHandle(hFile);
		MessageBox(L"申请内存空间失败!");
		return;
	}

	DWORD dwRet;

	if (!ReadFile(hFile, lpFileImage, dwSize, &dwRet, NULL))
	{
		CloseHandle(hFile);
		VirtualFree(lpFileImage, 0, MEM_RELEASE);
		MessageBox(L"加载文件失败!");
		return;
	}

	if (PE_fileTest(lpFileImage, dwSize))
	{
		wchar_t szBackUp[MAX_PATH] = { 0 };
		wsprintf(szBackUp, L"%s.bak", szFileName);
		CopyFile(szFileName, szBackUp, FALSE);
		showPE_info(lpFileImage, dwSize);
		SetWindowText(szFileName);
	}
	CloseHandle(hFile);

}

BOOL Cinfo_PEDlg::PE_fileTest(PVOID lpImage, DWORD dwSize)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	if (pDos->e_magic != IMAGE_DOS_SIGNATURE)
	{
		MessageBox(L"非PE文件!");
		return FALSE;
	}
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((size_t)lpImage + pDos->e_lfanew);
	if (pNT->Signature != IMAGE_NT_SIGNATURE)
	{
		MessageBox(L"非PE文件");
		return FALSE;
	}
	return TRUE;
}

void Cinfo_PEDlg::showPE_info(PVOID lpImage, DWORD dwSize)
{
	m_SectionInfo.DeleteAllItems();
	m_ExtendedHeader.DeleteAllItems();
	m_DataDir.DeleteAllItems();

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)lpImage;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((size_t)lpImage + pDos->e_lfanew);

	PIMAGE_FILE_HEADER pFileHeader = &(pNt->FileHeader);
	wchar_t szMachine[32] = { 0 };
	switch (pFileHeader->Machine)
	{
	case IMAGE_FILE_MACHINE_UNKNOWN:
	{
									   StringCchCopy(szMachine, 32, L"Unknown");
									   break;
	}
	case IMAGE_FILE_MACHINE_IA64:
	{
									StringCchCopy(szMachine, 32, L"Intel 64");
									break;
	}
	case IMAGE_FILE_MACHINE_I386:
	{
									StringCchCopy(szMachine, 32, L"Intel 386");
									break;
	}
	case IMAGE_FILE_MACHINE_AMD64:
	{
									 StringCchCopy(szMachine, 32, L"AMD64 K8");
									 break;
	}
	case IMAGE_FILE_MACHINE_POWERPC:
	{
									   StringCchCopy(szMachine, 32, L"IBM POWERPC");
									   break;
	}
	case IMAGE_FILE_MACHINE_ARM:
	{
								   StringCchCopy(szMachine, 32, L"ARM Little-Endian");
								   break;
	}
	default:
		break;
	}

	CWnd *pWnd = nullptr;
	pWnd = GetDlgItem(IDC_EDIT1);
	pWnd->SetWindowText(szMachine);

	wchar_t szNumofSection[2];
	_itow_s(pFileHeader->NumberOfSections, szNumofSection, 10);
	pWnd = GetDlgItem(IDC_EDIT2);
	pWnd->SetWindowText(szNumofSection);
	pWnd = nullptr;

	PIMAGE_OPTIONAL_HEADER pOptional = &(pNt->OptionalHeader);
	wchar_t szFileType[16] = { 0 };
	switch (pOptional->Magic)
	{
	case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
	{
										  StringCchCopy(szFileType, 16, L"32位可执行映像");
										  break;
	}
	case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
	{
										  StringCchCopy(szFileType, 16, L"64位可执行映像");
										  break;
	}
	case IMAGE_ROM_OPTIONAL_HDR_MAGIC:
	{
										 StringCchCopy(szFileType, 16, L"ROM映像");
										 break;
	}
	default:
		break;
	}

	if (pOptional->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		isx64 = TRUE;
	}

	m_ExtendedHeader.Additem(0, 2, L"文件类型", szFileType);

	wchar_t szLinkerVersion[10] = { 0 };
	wsprintf(szLinkerVersion, L"%2d.%3d", pOptional->MajorLinkerVersion, pOptional->MinorLinkerVersion);
	m_ExtendedHeader.Additem(1, 2, L"连接器版本", szLinkerVersion);

	wchar_t szEntryPoint[32] = { 0 };
	if (isx64)
	{
		wsprintf(szEntryPoint, L"%#08X [%#08X]", pOptional->AddressOfEntryPoint + pOptional->ImageBase, pOptional->AddressOfEntryPoint);
	}
	else
	{
		wsprintf(szEntryPoint, L"%#08X [%#08X]", pOptional->AddressOfEntryPoint + pOptional->ImageBase, pOptional->AddressOfEntryPoint);
	}
	m_ExtendedHeader.Additem(2, 2, L"程序入口点", szEntryPoint);

	wchar_t szCodeSegAddr[32] = { 0 };
	if (isx64)
	{
		wsprintf(szCodeSegAddr, L"%#016X", pOptional->BaseOfCode);
	}
	else
	{
		wsprintf(szCodeSegAddr, L"%#08X", pOptional->BaseOfCode);
	}
	m_ExtendedHeader.Additem(3, 2, L"代码段地址", szCodeSegAddr);


	if (!isx64)
	{
		wchar_t szDataSegAddr[32] = { 0 };
		wsprintf(szDataSegAddr, L"%#08X", pOptional->BaseOfData);
		m_ExtendedHeader.Additem(4, 2, L"数据段地址", szDataSegAddr);
	}

	if (isx64)
	{
		ULONGLONG *ull = (ULONGLONG*)((size_t)pOptional + sizeof(DWORD)* 6);
		wchar_t szImageBase[32] = { 0 };
		wsprintf(szImageBase, L"%#016X", *ull);
		m_ExtendedHeader.Additem(5, 2, L"映像基址", szImageBase);
	}
	else
	{
		wchar_t szImageBase[32] = { 0 };
		wsprintf(szImageBase, L"%#08X", pOptional->ImageBase);
		m_ExtendedHeader.Additem(5, 2, L"映像基址", szImageBase);
	}


	wchar_t szFileAlignment[32] = { 0 };
	if (isx64)
	{
		wsprintf(szFileAlignment, L"%#016X", pOptional->FileAlignment);
	}
	else
	{
		wsprintf(szFileAlignment, L"%#08X", pOptional->FileAlignment);
	}
	m_ExtendedHeader.Additem(6, 2, L"文件粒度", szFileAlignment);

	wchar_t szSectionAlignment[32] = { 0 };
	if (isx64)
	{
		wsprintf(szSectionAlignment, L"%#016X", pOptional->SectionAlignment);
	}
	else
	{
		wsprintf(szSectionAlignment, L"%#08X", pOptional->SectionAlignment);
	}
	m_ExtendedHeader.Additem(7, 2, L"内存粒度", szSectionAlignment);

	wchar_t szSysVersion[32] = { 0 };
	wsprintf(szSysVersion, L"%2d.%2d", pOptional->MajorOperatingSystemVersion, pOptional->MinorOperatingSystemVersion);
	m_ExtendedHeader.Additem(8, 2, L"系统版本号", szSysVersion);

	wchar_t szTotalImageSize[32] = { 0 };
	if (isx64)
	{
		wsprintf(szTotalImageSize, L"%#016X", pOptional->SizeOfImage);
	}
	else
	{
		wsprintf(szTotalImageSize, L"%#08X", pOptional->SizeOfImage);
	}
	m_ExtendedHeader.Additem(9, 2, L"映像总大小", szTotalImageSize);

	wchar_t szPESize[32] = { 0 };
	if (isx64)
	{
		wsprintf(szPESize, L"%#016X", pOptional->SizeOfHeaders);
	}
	else
	{
		wsprintf(szPESize, L"%#08X", pOptional->SizeOfHeaders);
	}
	m_ExtendedHeader.Additem(10, 2, L"PE头大小", szPESize);

	wchar_t szDataDirName[15][16] = { L"导出表", L"导入表", L"资源", L"异常", L"安全", L"基址重定位", L"调试", L"版权", L"全局指针", L"TLS", L"载入配置", L"绑定输入", L"导入地址(IAT)", L"延迟导入", L"COM描述符" };

	PDWORD pd = nullptr;
	PIMAGE_DATA_DIRECTORY pDataDir = nullptr;

	if (isx64)
	{
		pd = (PDWORD)((size_t)pOptional + sizeof(DWORD)* 27);
		pDataDir = (PIMAGE_DATA_DIRECTORY)((size_t)pOptional + sizeof(DWORD)* 28);
	}
	else
	{
		pd = &pOptional->NumberOfRvaAndSizes;
		pDataDir = (PIMAGE_DATA_DIRECTORY)pOptional->DataDirectory;
	}
	for (DWORD i = 0; i < *pd; i++)
	{
		wchar_t szDirName[16] = { 0 };
		if (i >= 14)
		{
			StringCchCopy(szDirName, 16, L"未知");
		}
		else
		{
			StringCchCopy(szDirName, 16, szDataDirName[i]);
		}
		wchar_t szDirAddr[32] = { 0 };
		wchar_t szDirSize[32] = { 0 };
		wsprintf(szDirAddr, L"%#08X", pDataDir[i].VirtualAddress);
		wsprintf(szDirSize, L"%#08X", pDataDir[i].Size);
		m_DataDir.Additem(i, 3, szDirName, szDirAddr, szDirSize);
	}

	for (DWORD i = 0; i < pFileHeader->NumberOfSections; i++)
	{
		PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
		char szSectionName[32] = { 0 };
		wchar_t szOffset[32] = { 0 };
		wchar_t szFileSize[32] = { 0 };
		wchar_t szRVA[32] = { 0 };
		wchar_t szMemSize[32] = { 0 };
		sprintf_s(szSectionName, "%s", pSection[i].Name);
		wsprintf(szOffset, L"%#08X", pSection[i].PointerToRawData);
		wsprintf(szFileSize, L"%#08X", pSection[i].SizeOfRawData);
		wsprintf(szRVA, L"%#08X", pSection[i].VirtualAddress);
		wsprintf(szMemSize, L"%#08X", pSection[i].Misc.VirtualSize);
		wchar_t szWideSectionName[32] = { 0 };
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szSectionName, -1, szWideSectionName, 32);
		m_SectionInfo.Additem(i, 5, szWideSectionName, szOffset, szFileSize, szRVA, szMemSize);
	}

}

void Cinfo_PEDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	GetCursorPos(&point);
	if (lpFileImage == nullptr || dwSize == 0)
	{
		menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32771, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32772, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32773, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32774, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32775, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	else
	{
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

		if (pDir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 || pDir[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
		{
			menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32772, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		if (pDir[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress == 0 || pDir[IMAGE_DIRECTORY_ENTRY_IMPORT].Size == 0)
		{
			menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32773, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		if (pDir[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress == 0 || pDir[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size == 0)
		{
			menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32774, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		if (pDir[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress == 0 || pDir[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size == 0)
		{
			menu.GetSubMenu(0)->EnableMenuItem(ID_CALC_32775, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
	}
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON
		| TPM_RIGHTBUTTON, point.x, point.y, this, NULL);

	//CDialogEx::OnRButtonUp(nFlags, point);
}


void Cinfo_PEDlg::OnCalc32771()
{
	// TODO: Add your command handler code here
	CCalc calc;
	calc.lpFileImage = lpFileImage;
	calc.dwSize = dwSize;
	calc.isx64 = isx64;
	calc.DoModal();

}



BOOL Cinfo_PEDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	if (lpFileImage != nullptr)
	{
		VirtualFree(lpFileImage, 0, MEM_RELEASE);
		lpFileImage = nullptr;
	}
	return CDialogEx::DestroyWindow();
}


void Cinfo_PEDlg::OnCalc32772()
{
	// TODO: Add your command handler code here
	CExportTable et;
	et.lpImage = lpFileImage;
	et.dwSize = dwSize;
	et.isx64 = isx64;
	et.DoModal();
}


void Cinfo_PEDlg::OnCalc32773()
{
	// TODO: Add your command handler code here
	CImportTable it;
	it.lpFileImage = lpFileImage;
	it.dwSize = dwSize;
	it.isx64 = isx64;
	it.DoModal();
}


void Cinfo_PEDlg::OnCalc32774()
{
	// TODO: Add your command handler code here
	CResourceTable rt;
	rt.lpFileImage = lpFileImage;
	rt.dwSize = dwSize;
	rt.isx64 = isx64;
	rt.DoModal();
}


void Cinfo_PEDlg::OnCalc32775()
{
	// TODO: Add your command handler code here
	CRelocationTable relocT;
	relocT.lpFileImage = lpFileImage;
	relocT.dwSize = dwSize;
	relocT.isx64 = isx64;
	relocT.DoModal();

}


void Cinfo_PEDlg::OnCalc32776()
{
	// TODO: Add your command handler code here
	OPENFILENAME  stOpenFile;
	WCHAR		  szFileName[MAX_PATH] = { 0 };
	ZeroMemory(&stOpenFile, sizeof(OPENFILENAME));
	stOpenFile.lStructSize = sizeof(OPENFILENAME);
	stOpenFile.hwndOwner = m_hWnd;
	stOpenFile.lpstrFilter = L"可执行文件(*.exe)\0*.exe\0所有文件(*.dll,*.exe,*.sys,*.ocx,*.com,*.scr,*.cpl)\0*.dll;*.exe;*.sys;*.ocx;*.com;*.scr;*.cpl\0";
	//stOpenFile.lpstrFilter	= L"PE File\0*.exe\0*.dll\0"; // 格式有错 ？？？？
	stOpenFile.lpstrFile = szFileName;
	stOpenFile.nMaxFile = MAX_PATH;
	stOpenFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	BOOL isOpenFile = GetOpenFileName(&stOpenFile);

	if (isOpenFile)
	{
		getPEinfo(szFileName);
	}
	else
	{
		return;
	}
}


void Cinfo_PEDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	if (lpFileImage != nullptr)
	{
		VirtualFree(lpFileImage, 0, MEM_RELEASE);
		lpFileImage = nullptr;
	}
	// TODO: Add your message handler code here
}
