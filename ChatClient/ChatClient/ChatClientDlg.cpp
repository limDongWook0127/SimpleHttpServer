
// ChatClientDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"
#include <algorithm> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.
UINT SendFileThreadProc(LPVOID pParam); //전역함수 선언
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
	ON_MESSAGE(WM_USER + 200, &CChatClientDlg::OnChatMessage)
	ON_MESSAGE(WM_FILE_SEND_DONE, &CChatClientDlg::OnFileSendDone)
	ON_MESSAGE(WM_FILE_SEND_ERROR, &CChatClientDlg::OnFileSendError)
	ON_MESSAGE(WM_FILE_SEND_PROGRESS, &CChatClientDlg::OnFileSendProgress)

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
	StartWorkerThread();
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
		m_FilePath = dlg.GetPathName();
		CWinThread* Thread = AfxBeginThread(SendFileThreadProc, this);
		if (Thread)
			m_sendThreadHandle = Thread->m_hThread;
	}
}

void CChatClientDlg::OnListDblClk()
{
	int sel = m_List.GetCurSel();        // 1. 선택된 항목 인덱스 얻기
	if (sel == LB_ERR) return;           // 2. 아무것도 선택 안 했으면 return

	auto it = m_FileDownloadMap.find(sel);
	if (it == m_FileDownloadMap.end()) return;   // 3. 파일 항목 아니면 return

	CString filePath = it->second;               // 4. 임시파일 경로 얻기
	CString fileName = filePath.Mid(filePath.ReverseFind(L'\\') + 1); // 5. 파일명 추출 \는 이스케이프 문자이므로 실제 문자 \를 의미할땐 \\써야함
	//mid는(숫자) 숫자부터 끝까지 잘라내는 함수
	// 6. 저장 다이얼로그(“다른 이름으로 저장”)
	CString ext = filePath.Mid(filePath.ReverseFind('.') + 1);
	CFileDialog dlg(FALSE, ext, fileName, OFN_OVERWRITEPROMPT, L"All Files (*.*)|*.*||", this);//TRUE열기 FALSE저장, 확장자, 파일이름, 중복처리, 
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

void CChatClientDlg::StartWorkerThread()
{
	m_bThreadRun = true;
	m_workerThread = std::thread([this]()
		{
			while (m_bThreadRun)
			{
				std::vector<BYTE> data;
				{
					std::unique_lock<std::mutex> lock(m_queueMutex);
					m_cv.wait(lock, [this] { return !m_recvQueue.empty() || !m_bThreadRun; });
					//wait을 통해서 m_workerThread 스레드를 잠재움, 스레드는 cpu에서 완전히 제외되고 대기상태로빠짐
					//notify_one을 하면 잠자고 있던 스레드가 깨어나고 wait 안의 조건을 확인함(리턴값이 true면 스레드가 일함)
					//조건이 만족되면 wait에서빠져나가서 아래 코드 실행



					//m_recvQueue에 데이터가 생기거나, m_bThreadRun이 false일 때까지 스레드를 잠재워라. 
					//큐에 메시지 들어오거나 쓰레드런변수 true되면 즉시 작동
					//큐가 비어있을때는 와일문에서 무한루프 돌면서 계속 큐를 확인하지 않도록 함
					//큐가 비어있지 않으면 깨어나라(쓰레드 작동시켜라)!, 쓰레드런이 false이면 일어나라


					if (!m_bThreadRun)
						return;

					data = std::move(m_recvQueue.front());
					m_recvQueue.pop();

				}
				if (CConnectSocket::IsFileHeader(data.data(), (int)data.size()))
				{
					// 워커 스레드 내에서 직접 파일 수신 처리
					this->HandleReceivedFileInWorker(data);
				}
				else
				{
					// 일반 채팅 처리
					data.push_back(0);
					data.push_back(0);
					CString msg((LPCTSTR)data.data());
					PostMessage(WM_USER + 200, (WPARAM)new CString(msg), 0);
				}


			}
		});
}

LRESULT CChatClientDlg::OnChatMessage(WPARAM wParam, LPARAM lParam)
{

	CString* pStr = reinterpret_cast<CString*>(wParam);
	if (!pStr) return 0;

	int idx = m_List.AddString(*pStr);
	m_List.SetCurSel(idx);
	delete pStr;
	return 0;
}

void CChatClientDlg::HandleReceivedFileInWorker(std::vector<BYTE>& firstChunk)
{
	// 1. 파일 헤더 추출
	FileHeader header{};
	int copied = std::min<int>((int)firstChunk.size(), (int)sizeof(FileHeader));
	memcpy(&header, firstChunk.data(), copied);
	
	// 2. body가 포함되어 있으면 포인터 지정
	BYTE* body = nullptr;
	int bodyLen = 0;
	if ((int)firstChunk.size() > (int)sizeof(FileHeader)) 
	{
		body = firstChunk.data() + sizeof(FileHeader);
		bodyLen = (int)firstChunk.size() - (int)sizeof(FileHeader);
	}

	// 3. 파일 저장 경로 설정
	TCHAR tempDir[MAX_PATH] = { 0 };
	GetTempPath(MAX_PATH, tempDir);
	CStringW FileName = CStringW(CA2W(header.fileName, CP_UTF8));

	// (2) filePath도 유니코드로!
	CString filePath;
	filePath.Format(L"%s%s", tempDir, FileName.GetString());
	AfxMessageBox(filePath);
	CFile file;
	if (!file.Open(filePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) 
	{
		AfxMessageBox(L"파일 열기 실패");
		return;
	}

	// 4. body 일부 저장
	if (body && bodyLen > 0)
		file.Write(body, bodyLen);

	int remain = header.fileSize - bodyLen;

	// 5. 큐에서 계속 받아서 저장
	while (remain > 0)
	{
		std::vector<BYTE> chunk;
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_cv.wait(lock, [this] { return !m_recvQueue.empty() || !m_bThreadRun; });
			if (!m_bThreadRun)
				break;

			chunk = std::move(m_recvQueue.front());
			m_recvQueue.pop();
		}

		int want = std::min<int>(remain, (int)chunk.size());
		file.Write(chunk.data(), want);
		remain -= want;

		// ⚠️ chunk에 예상보다 많은 데이터가 있을 경우 (다음 메시지 섞임)
		if ((int)chunk.size() > want)
		{
			std::vector<BYTE> leftover(chunk.begin() + want, chunk.end());
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_recvQueue.push(std::move(leftover));
			m_cv.notify_one();
		}
	}

	file.Close();

	// 6. UI 메시지 보내기
	CString* pStr = new CString;
	CStringW wFileName = CStringW(CA2W(header.fileName, CP_UTF8));
	pStr->Format(L"%s [다운로드(더블클릭)]", wFileName.GetString());
	int idx = m_List.GetCount();
	m_FileDownloadMap[idx] = filePath;
	PostMessage(WM_USER + 200, (WPARAM)pStr, 0);
	
}

LRESULT CChatClientDlg::OnFileSendDone(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox(L"파일 전송이 완료되었습니다!");
	return 0;
}

LRESULT CChatClientDlg::OnFileSendError(WPARAM wParam, LPARAM lParam)
{
	AfxMessageBox(L"파일 전송 중 오류가 발생했습니다.");
	return 0;
}


LRESULT CChatClientDlg::OnFileSendProgress(WPARAM wParam, LPARAM lParam)
{
	int percent = (int)wParam;
	CString msg;
	// 확실한 접두어로 구분
	msg.Format(L"[PROGRESS] 파일 전송 진행: %d%%", percent);

	// 리스트의 뒤에서부터 "진행률 메시지"를 찾아서 삭제
	int count = m_List.GetCount();
	for (int i = count - 1; i >= 0; --i)
	{
		CString line;
		m_List.GetText(i, line);
		if (line.Left(10) == L"[PROGRESS]")
		{
			m_List.DeleteString(i);
			break;
		}
	}

	int idx = m_List.AddString(msg);
	m_List.SetCurSel(idx);
	return 0;
}



//UINT SendFileThreadProc(LPVOID pParam) 
//{
//	CChatClientDlg* pDlg = (CChatClientDlg*)pParam; //이 포인터로 m_FilePath, m_Socket 등에 접근할 수 있음
//
//	if (!::IsWindow(pDlg->GetSafeHwnd()))
//		return 0;
//
//	FILE* fp = nullptr;
//	_wfopen_s(&fp, pDlg->m_FilePath, L"rb");
//	
//
//	//errno_t _wfopen_s(FILE** pFile, const wchar_t* filename, const wchar_t* mode); 반환값은 성공 -> 0 , 실패 ->에러코드
//	//FILE** pFile : 열린 파일을 가리키는 포인터를 저장할 변수의 주소
//	//filename: 열 파일 경로(wchar_t*, 즉 wide string)
//	//mode : 열 모드(L"rb" → read binary mode)
//	if (!fp)
//	{
//		::PostMessage(pDlg->m_hWnd, WM_FILE_SEND_ERROR, 0, 0);
//		return 0;
//	}
//	FileHeader header{}; //내부 멤버들을 0으로 초기화하겠다는 의미로{}쓰는것
//	header.type = 1;
//	fseek(fp, 0, SEEK_END); //fseek 파일 포인터를 이동시킴(끝으로)
//	header.fileSize = (uint32_t)ftell(fp); //현재 위치를 바이트 단위로 알려줌
//	fseek(fp, 0, SEEK_SET);
//	CString fileName = PathFindFileName(pDlg->m_FilePath); //파일명만 추출하는 함수
//	//ex)
//	//LPCTSTR path = _T("C:\\Users\\donguk\\Documents\\example.txt");
//	//LPCTSTR fileName = PathFindFileName(path);
//	// fileName == "example.txt"
//	CT2A fileNameA(fileName, CP_UTF8); //mfc에서 유니코드 문자열을 멀티바이트 문자열로 변환 (wchar_t* → char* 로 변환하는 임시 변환 객체)
//	strcpy_s(header.fileName, _countof(header.fileName), fileNameA); //char*기반 함수라서 멀티바이트 문자열만 적용가능해서 위에서 변환한것
//
//	BYTE* pHeader = new BYTE[sizeof(FileHeader)];
//	memcpy(pHeader, &header, sizeof(FileHeader));
//	::PostMessage(pDlg->GetSafeHwnd(), WM_SEND_FILE_CHUNK, (WPARAM)pHeader, sizeof(FileHeader));
//
//	uint32_t fileSize = header.fileSize;
//	uint32_t sentSize = 0;
//	int lastPercent = 0; // 중복 메시지 방지용
//
//	// 2. 파일 본문 전송
//	BYTE buffer[64*1024];
//	while (!feof(fp))
//	{
//		int nRead = (int)fread(buffer, 1, sizeof(buffer), fp);
//		if (nRead > 0)
//		{
//			BYTE* pChunk = new BYTE[nRead];
//			memcpy(pChunk, buffer, nRead);
//
//			// 1. 항상 청크는 보내줘야 함
//			::PostMessage(pDlg->GetSafeHwnd(), WM_SEND_FILE_CHUNK, (WPARAM)pChunk, nRead);
//
//			// 2. 진행률은 1% 올라갈 때마다 한 번만 보내기
//			sentSize += nRead;
//			int percent = 0;
//			if (fileSize > 0) percent = (int)((sentSize * 100) / fileSize);
//			if (percent > lastPercent)
//			{
//				::PostMessage(pDlg->GetSafeHwnd(), WM_FILE_SEND_PROGRESS, (WPARAM)percent, 0);
//				lastPercent = percent;
//			}
//			Sleep(1);
//		}
//	}
//
//	fclose(fp);
//	if (::IsWindow(pDlg->GetSafeHwnd()))
//		::PostMessage(pDlg->m_hWnd, WM_FILE_SEND_DONE, 0, 0);
//	return 0;
//}
//송신(파일전송 누르면 멀티스레드에서 호출)
UINT SendFileThreadProc(LPVOID pParam) // LPVOID는 void*와 동일한 자료형
{
	CChatClientDlg* pDlg = (CChatClientDlg*)pParam; // 이 포인터로 m_FilePath, m_Socket 등에 접근할 수 있음
	if (!pDlg) return 0;

	// 윈도우 핸들 체크는 진행률 갱신할 때만 필요
	if (!::IsWindow(pDlg->GetSafeHwnd()))
		return 0;

	FILE* fp = nullptr;
	_wfopen_s(&fp, pDlg->m_FilePath, L"rb");
	// m_FilePath에 해당하는 파일을 읽기전용으로 안전하게 열고 fp 포인터에 결과를 저장하는 코드
	// errno_t _wfopen_s(FILE** pFile, const wchar_t* filename, const wchar_t* mode);
	// 반환값: 성공 -> 0, 실패 -> 에러코드
	// FILE** pFile : 열린 파일을 가리키는 포인터를 저장할 변수의 주소
	// filename: 열 파일 경로(wchar_t*, 즉 wide string)
	// mode : 열 모드(L"rb" → read binary mode)
	if (!fp)
	{
		::PostMessage(pDlg->m_hWnd, WM_FILE_SEND_ERROR, 0, 0);
		return 0;
	}

	// === 1. 파일 헤더 전송 ===
	FileHeader header{}; // 내부 멤버들을 0으로 초기화하겠다는 의미로 {} 사용
	header.type = 1;

	fseek(fp, 0, SEEK_END); // fseek : 파일 포인터를 이동시킴 (여기서는 끝으로 이동)
	header.fileSize = (uint32_t)ftell(fp); // ftell : 현재 위치를 바이트 단위로 알려줌
	fseek(fp, 0, SEEK_SET); // 파일 포인터를 다시 처음으로 이동

	CString fileName = PathFindFileName(pDlg->m_FilePath); // 파일명만 추출하는 함수
	// 예)
	// LPCTSTR path = _T("C:\\Users\\donguk\\Documents\\example.txt");
	// LPCTSTR fileName = PathFindFileName(path);
	// fileName == "example.txt"
	CT2A fileNameA(fileName, CP_UTF8); // MFC에서 유니코드 문자열을 멀티바이트 문자열로 변환 (wchar_t* → char* 로 변환하는 임시 변환 객체)
	strcpy_s(header.fileName, _countof(header.fileName), fileNameA); // char* 기반 함수라서 멀티바이트 문자열만 적용 가능해서 위에서 변환한 것

	// 헤더를 직접 전송
	if (pDlg->m_Socket.SendAll(&header, sizeof(FileHeader)) == SOCKET_ERROR)
	{
		fclose(fp);
		::PostMessage(pDlg->m_hWnd, WM_FILE_SEND_ERROR, 0, 0);
		return 0;
	}

	// === 2. 파일 본문 전송 ===
	uint32_t fileSize = header.fileSize;
	uint32_t sentSize = 0;
	int lastPercent = 0; // 중복 메시지 방지용

	BYTE buffer[64 * 1024]; // 64KB 버퍼
	while (!feof(fp))
	{
		int nRead = (int)fread(buffer, 1, sizeof(buffer), fp);
		if (nRead <= 0) break;

		// 읽은 내용을 직접 전송
		if (pDlg->m_Socket.SendAll(buffer, nRead) == SOCKET_ERROR)
		{
			fclose(fp);
			::PostMessage(pDlg->m_hWnd, WM_FILE_SEND_ERROR, 0, 0);
			return 0;
		}

		// 진행률은 1% 올라갈 때마다 한 번만 보내기
		sentSize += nRead;
		int percent = 0;
		if (fileSize > 0) percent = (int)((sentSize * 100) / fileSize);
		if (percent > lastPercent)
		{
			::PostMessage(pDlg->m_hWnd, WM_FILE_SEND_PROGRESS, (WPARAM)percent, 0);
			lastPercent = percent;
		}
	}

	fclose(fp);

	// 완료 알림
	if (::IsWindow(pDlg->GetSafeHwnd()))
		::PostMessage(pDlg->m_hWnd, WM_FILE_SEND_DONE, 0, 0);

	return 0;
}