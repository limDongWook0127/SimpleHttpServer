#include "pch.h"
#include "CListenSocket.h"
#include "CClientSocket.h"
CListenSocket::CListenSocket()
{
}
CListenSocket::~CListenSocket()
{
	POSITION pos = m_ptrClientSocketList.GetHeadPosition();
	while (pos != NULL)
	{
		CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL)
		{
			pClient->ShutDown();
			pClient->Close();
			delete pClient;
		}
	}

	m_ptrClientSocketList.RemoveAll();

	// 서버 소켓 종료
	if (m_hSocket != INVALID_SOCKET)
	{
		ShutDown(); //송수신 차단(WinSock shutdown() 호출 래핑)
		Close(); //m_hSocket = INVALID_SOCKET 으로 리셋.
	}
}
void CListenSocket::OnAccept(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	CClientSocket* pClient = new CClientSocket;

	if (Accept(*pClient))
	{
		pClient->SetListenSocket(this); //서버의 클라이언트 소켓(pclient)을 만든 리슨 소켓은 이거야 라고 설정하는것
		m_ptrClientSocketList.AddTail(pClient);
	}

	else
	{
		delete pClient;
		AfxMessageBox(L"ERROR: Failed to accpet new client!");
	}

	CAsyncSocket::OnAccept(nErrorCode);
}

void CListenSocket::CloseClientSokcet(CSocket* pClient) //프로그램 끝날때까지 메모리/핸들에 
//남아있지 않도록 한명만 나갔을때 즉시 정리할 수 있도록 하는 코드
{
	POSITION pos;
	pos = m_ptrClientSocketList.Find(pClient);
	if (pos != NULL)
	{
		if (pClient != NULL)
		{
			pClient->ShutDown();
			pClient->Close();
		}

		m_ptrClientSocketList.RemoveAt(pos);
		delete pClient;
	}
}

void CListenSocket::SendChatDataAll(TCHAR* pszMessage)
{
	POSITION pos;
	pos = m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;

	while (pos != NULL)
	{
		pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (pClient != NULL)
		{
			pClient->Send(pszMessage, lstrlen(pszMessage) * 2);
		}
	}
}

void CListenSocket::SendBinaryToAll(const void* buf, int len, CClientSocket* pExcept)
{
	POSITION pos = m_ptrClientSocketList.GetHeadPosition();
	while (pos)
	{
		CClientSocket* pClient = (CClientSocket*)m_ptrClientSocketList.GetNext(pos);
		if (!pClient) continue;
		if (pExcept && pClient == pExcept)
			continue;
		if (pClient->SendAll(buf, len) == SOCKET_ERROR)
			CloseClientSokcet(pClient);
	}
}
