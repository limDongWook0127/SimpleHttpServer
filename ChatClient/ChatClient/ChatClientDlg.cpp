
// ChatClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatClientDlg 대화 상자



CChatClientDlg::CChatClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CChatClientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CChatClientDlg::OnBnClickedButton2)
	ON_LBN_DBLCLK(IDC_LIST1, &CChatClientDlg::OnListDblClk)
END_MESSAGE_MAP()


// CChatClientDlg 메시지 처리기

BOOL CChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_Socket.Create();
	if (m_Socket.Connect(L"127.0.0.1", 21000) == FALSE)
	{
		AfxMessageBox(L"ERROR: Failed to Connect Server");
		PostQuitMessage(0);
		return FALSE;
	}

	CNickNameDlg dlg;
	if (dlg.DoModal() == IDOK) //다이얼 로그를 모달 모드로 띄움, 사용자가 확인 누르면 idok리턴 ->아래코드 실행
	{
		m_NickName = dlg.m_NickName;
		m_Socket.Send((LPVOID)(LPCTSTR)m_NickName, m_NickName.GetLength() * sizeof(TCHAR));
		if (m_NickName.IsEmpty())
		{
			AfxMessageBox(L"닉네임을 입력하지 않으면 접속할 수 없습니다.");
			EndDialog(IDCANCEL);  // 창 종료
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox(L"닉네임 입력이 취소되었습니다.");
		EndDialog(IDCANCEL);
		return FALSE;
	}


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CChatClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CChatClientDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (GetFocus() == GetDlgItem(IDC_EDIT1))
		{
			OnBnClickedButton1();
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CChatClientDlg::OnOK()
{

}




void CChatClientDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_Edit.GetWindowText(m_strMessage);
	m_Socket.Send((LPVOID)(LPCTSTR)m_strMessage, m_strMessage.GetLength() * 2);

	m_strMessage = L"";
	m_Edit.SetWindowText(L""); //m_Edit창 비워주기(메시지입력하는창)
}





void CChatClientDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg(TRUE);
	if (dlg.DoModal() == IDOK)
	{
		m_Socket.SendFile(dlg.GetPathName());
	}
}

void CChatClientDlg::OnListDblClk()
{
	int sel = m_List.GetCurSel();        // 1. 선택된 항목 인덱스 얻기
	if (sel == LB_ERR) return;           // 2. 아무것도 선택 안 했으면 return

	auto it = m_FileDownloadMap.find(sel);
	if (it == m_FileDownloadMap.end()) return;   // 3. 파일 항목 아니면 return

	CString filePath = it->second;               // 4. 임시파일 경로 얻기
	CString fileName = filePath.Mid(filePath.ReverseFind('\\') + 1); // 5. 파일명 추출 \는 이스케이프 문자이므로 실제 문자 \를 의미할땐 \\써야함
	//mid는(숫자) 숫자부터 끝까지 잘라내는 함수
	// 6. 저장 다이얼로그(“다른 이름으로 저장”)
	CString ext = filePath.Mid(filePath.ReverseFind('.') + 1);

	CFileDialog dlg(FALSE, ext, fileName, OFN_OVERWRITEPROMPT, L"All Files (*.*)|*.*||", this);
	if (dlg.DoModal() != IDOK) return;           // 7. 취소하면 끝
	
	CString destPath = dlg.GetPathName();        // 8. 사용자가 선택한 경로
	if (!CopyFile(filePath, destPath, FALSE))    // 9. 임시파일을 해당 경로로 복사
	{
		AfxMessageBox(L"복사 실패");
		return;
	}
	CString msg;
	msg.Format(L"[저장 완료] %s", destPath.GetString()); // 10. 저장 성공 메시지
	m_List.AddString(msg);
}