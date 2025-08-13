#include "pch.h"
#include "CClientSocket.h"
#include "CListenSocket.h"
#include "ChatServerDlg.h"
#include <vector>
CClientSocket::CClientSocket()
{
	m_pListenSocket = NULL;
}

CClientSocket::~CClientSocket()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		ShutDown();
		Close();
	}
}

void CClientSocket::SetListenSocket(CListenSocket* pSocket)
{
	m_pListenSocket = pSocket;
}

void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CSocket::OnClose(nErrorCode);

	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->CloseClientSokcet(this);

}

void CClientSocket::OnReceive(int nErrorCode)
{
	BYTE buffer[4096] = { 0 };
	int nLen = Receive(buffer, sizeof(buffer));//버퍼에 남은 데이터 다 읽고 나면 다음 Receive()에서 0 반환
	// nLen=0 : 상대가 정상적으로 연결을 끊었다(FIN 보냄)
	// nLen>0 : 실제로 읽어온 바이트 수
	//     -1 : SOCKET_ERROR(-1) : 오류
	if (nLen <= 0)
	{
		m_pListenSocket->CloseClientSokcet(this); //nLen이 0이라는건 TCP 규약상 상대가 연결을 끊었다는 뜻
		CSocket::OnReceive(nErrorCode);
		return;
	}

	if (IsFileHeader(buffer, nLen))
	{
		HandleFileTransfer(buffer, nLen);
	}
	else if (!m_bNickNameReceived)
	{
		SetNickName(buffer, nLen);
	}
	else
	{
		HandleChatMessage(buffer, nLen);
	}

	CSocket::OnReceive(nErrorCode);
}

int CClientSocket::SendAll(const void* buf, int len)
//const void*는 “타입을 모르는(또는 신경 안 쓰는) 메모리 블록을 가리키는 읽기 전용 포인터”
{
	const char* p = (const char*)buf;
	int total = 0;
	while (total < len)
	{
		int n = Send(p + total, len - total);
		if (n == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		total += n;
	}
	return total;
}

int CClientSocket::RecvAll(void* buf, int len)
{
	char* p = (char*)buf;
	int total = 0;
	while (total < len)
	{
		int n = Receive(p + total, len - total);
		if (n <= 0) return n; // 0: 끊김, -1: 오류
		total += n;
	}
	return total;
}

void CClientSocket::SetNickName(BYTE* buffer, int nLen)
{
	m_bNickNameReceived = true;
	m_NickName = (LPCTSTR)buffer;
	

	CString msg;
	
	msg.Format(L"[%s] 님이 입장했습니다.", (LPCTSTR)m_NickName);

	CChatServerDlg* pMain = (CChatServerDlg*)AfxGetMainWnd();
	pMain->m_List.AddString(msg);
	pMain->m_List.SetCurSel(pMain->m_List.GetCount() - 1);

	CListenSocket* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->SendChatDataAll((TCHAR*)(LPCTSTR)msg);
}

bool CClientSocket::IsFileHeader(BYTE* buffer, int nLen)
{
	if (nLen < sizeof(FileHeader))
		return false;

	FileHeader* header = (FileHeader*)buffer;
	return header->type == 1;
}

void CClientSocket::HandleChatMessage(BYTE* buffer, int nLen)
{
	CString content = (LPCTSTR)buffer;

	CString msg;
	
	msg.Format(L"[%s] : %s", (LPCTSTR)m_NickName, (LPCTSTR)content);

	CChatServerDlg* pMain = (CChatServerDlg*)AfxGetMainWnd();
	pMain->m_List.AddString(msg);
	//pMain->m_List.SetCurSel(pMain->m_List.GetCount() - 1);

	auto* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->SendChatDataAll((TCHAR*)(LPCTSTR)msg);
}

void CClientSocket::HandleFileTransfer(BYTE* buffer, int nLen)
{
	// 1) 헤더 확보
	FileHeader header{};//이걸 잘모르겟네 explicit이랑 관련있다는데 우선 FileHeader header = {};랑 비슷하다고 생각
	int copied = min(nLen, (int)sizeof(FileHeader));
	//이렇게하는 이유는 TCP는 부분 수신이 가능해서 헤더 전체 32바이트가 안올 수도 있음
	//nLen은 12이고 sizeof(FileHeader)는 32로 올 수 있음
	
	memcpy(&header, buffer, copied); //메모리 복사memcpy(어디에, 무엇을, 얼마나)

	if (copied < (int)sizeof(FileHeader))//첫 Receive()로 받은 바이트가 헤더 크기보다 작으면, 아직 헤더가 “미완성” 상태.
	{
		int need = sizeof(FileHeader) - copied;
		if (RecvAll(((BYTE*)&header) + copied, need) <= 0)  //RecvAll이 0(끊김) 또는 음수(에러)를 반환하면 실패로 보고 빠져나옴.
			//((BYTE*)&header) + copied는 이미 채운 부분 다음 주소부터 이어서 쓰기 위한 포인터 연산.
		{
			AfxMessageBox(L"서버: 헤더 수신 실패");
			return;
		}
		buffer = nullptr;
		nLen = 0;
	}
	else
	{
		buffer += sizeof(FileHeader);// 1) 버퍼 포인터를 헤더 크기만큼 앞으로 이동 → 이제 buffer는 본문 시작 주소
		nLen -= sizeof(FileHeader);  // 2) 남은 길이에서 헤더 크기만큼 빼서, 본문 바이트 수만 남김
		
	}

	// 2) 로그
	CString msg;

	// UTF-8 → CString (Unicode) 변환
	CString fileNameW(CA2W(header.fileName, CP_UTF8));

	// 포맷 문자열은 %s (유니코드 CString용)
	msg.Format(L"[파일 수신] %s (%u bytes)", fileNameW.GetString(), header.fileSize);

	CChatServerDlg* pMain = (CChatServerDlg*)AfxGetMainWnd();
	pMain->m_List.AddString(msg);

	// 3) 헤더 브로드캐스트
	auto* pServerSocket = (CListenSocket*)m_pListenSocket;
	pServerSocket->SendBinaryToAll(&header, sizeof(header),this);

	// 4) 첫 chunk 브로드캐스트
	int firstChunk = min(nLen, (int)header.fileSize);
	//이번 Receive()로 이미 들어온 본문 크기(nLen)와 전체 파일 크기(header.fileSize) 중 작은 값만큼을 첫 번째로 보낼 덩어리 크기로 정함.
	if (firstChunk > 0 && buffer)
		pServerSocket->SendBinaryToAll(buffer, firstChunk,this);
	//첫 덩어리 크기가 0보다 크고, buffer가 유효하면
	//그 부분을 바로 모든 클라이언트에게 브로드캐스트.

	// 5) 남은 부분 계속 받아서 그대로 뿌리기
	int remain = header.fileSize - firstChunk;
	std::vector<BYTE> tmp(64 * 1024);

	while (remain > 0)
	{
		int want = min((int)tmp.size(), remain);
		int r = Receive(tmp.data(), want);
		if (r <= 0)
		{
			AfxMessageBox(L"서버: 파일 수신 중단/오류");
			return;
		}
		pServerSocket->SendBinaryToAll(tmp.data(), r,this);
		remain -= r;
	}
}