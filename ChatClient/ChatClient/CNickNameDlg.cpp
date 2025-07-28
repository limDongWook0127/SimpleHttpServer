#include "pch.h"
#include "CNickNameDlg.h"

IMPLEMENT_DYNAMIC(CNickNameDlg, CDialogEx)

BEGIN_MESSAGE_MAP(CNickNameDlg, CDialogEx)
    // 여기에 메시지 핸들러 등록 가능
END_MESSAGE_MAP()

void CNickNameDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_NickName); // 샘플 입력란으로 되어있는 공간에 문자를쓰면 m_NickName으로 저장하는 함수
}
