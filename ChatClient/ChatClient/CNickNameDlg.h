#pragma once
#include <afxdialogex.h>
#include "resource.h"
class CNickNameDlg :
    public CDialogEx
{
    DECLARE_DYNAMIC(CNickNameDlg)

public:
    CString m_NickName;
    CNickNameDlg(CWnd* pParent = nullptr)
        : CDialogEx(IDD_INPUTNICKNAME, pParent) { } //IDD_INPUTGNICKNAME 이라는 다이얼로그 리소스를 쓰겠다는 뜻
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
   

    DECLARE_MESSAGE_MAP()
};

