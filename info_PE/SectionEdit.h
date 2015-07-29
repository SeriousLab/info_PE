#pragma once


// CSectionEdit dialog

class CSectionEdit : public CDialogEx
{
	DECLARE_DYNAMIC(CSectionEdit)

public:
	CSectionEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSectionEdit();

	DWORD dwSize = 0;
	PVOID lpFileImage = nullptr;
	BOOL isx64 = FALSE;
	// Dialog Data
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
