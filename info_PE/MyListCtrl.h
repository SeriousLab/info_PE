#pragma once


// CMyListCtrl

class CMyListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CMyListCtrl)

public:
	CMyListCtrl();
	virtual ~CMyListCtrl();

	void  AddColumn(int nNum, ...)
	{
		//�����б������
		SetExtendedStyle(
			LVS_EX_FULLROWSELECT |
			LVS_EX_GRIDLINES |
			LVS_EX_AUTOAUTOARRANGE |
			LVS_EX_AUTOSIZECOLUMNS);
		//��ʼ������б�
		va_list vlist;
		va_start(vlist, nNum);
		//ѭ����ȡ����
		for (int i = 0; i < nNum; i++)
		{
			TCHAR* pName = va_arg(vlist, TCHAR*);
			int    nWidth = va_arg(vlist, int);
			//����һ��
			InsertColumn(i, pName, NULL, nWidth);
		}
		//����������ȡ
		va_end(vlist);
	}

	void Additem(int nItemNum,
		int nCount, ...)
	{
		// ��ʼ��
		va_list vlist;
		va_start(vlist, nCount);
		// ���һ�У������õ�һ����Ԫ������
		TCHAR* pText =
			va_arg(vlist, TCHAR*);
		int nIndex = InsertItem(nItemNum, pText);
		for (int i = 1; i < nCount; i++)
		{
			//���ñ���������Ԫ������
			pText =
				va_arg(vlist, TCHAR*);
			SetItemText(nIndex,
				i, pText);
		}
		//������ȡ����
		va_end(vlist);
	}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


