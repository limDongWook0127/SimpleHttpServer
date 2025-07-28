
// ChatClientDlg.h: 헤더 파일
//

#pragma once
#include "CConnectSocket.h"
#include "CNickNameDlg.h"
#include <map>

// CChatClientDlg 대화 상자
class CChatClientDlg : public CDialogEx
{
// 생성입니다.
public:
	CChatClientDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	CConnectSocket m_Socket;
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	//만든것
	virtual BOOL PreTranslateMessage(MSG* pMsg);//엔터키로 메시지 전송
	virtual void OnOK();

public:
	CListBox m_List;
	afx_msg void OnBnClickedButton1();
	CButton m_Edit;
	CString m_strMessage;
	CString m_NickName;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnListDblClk();
	std::map<int, CString> m_FileDownloadMap;
};
